/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <cmath> // std::abs

//I2C_MASTER_SDA
//I2C_MASTER_SCL

static const char *TAG = "esp-oled";


// I2C DEFINITIONS
#define I2C_MASTER_SCL_IO           CONFIG_I2C_MASTER_SCL      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           CONFIG_I2C_MASTER_SDA      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

// SSD1306 DEFINITIONS

// If I2C address is unspecified, use default
// (0x3C for 128x32 displays, 0x3D for all others).
// BAM: Using a 128x64 display, so 0x3D!
// EXCEPT!  3D gives nak, 3C gives ack, so, 3C must be correct???
#define SSD1306_I2C_ADDR 0x3C
#define FRAME_Y_RESOLUTION 64
#define FRAME_X_RESOLUTION 128

extern "C" {
    void app_main();
}
    
#define MAX_SHADES 17
static const uint8_t shade8x8[17*8]= // 17 shade patterns 8x8 pixels
    {
      0,  0,  0,  0,  0,  0,  0,  0, // 0
     17,  0,  0,  0, 17,  0,  0,  0,
     17,  0, 68,  0, 17,  0, 68,  0,
     68, 17, 68,  0, 68, 17, 68,  0,
     85,  0,170,  0, 85,  0,170,  0,
     68,170,  0,170, 68,170,  0,170,
     68,170, 17,170, 68,170, 17,170,
     85,136, 85,170, 85,136, 85,170,
     85,170, 85,170, 85,170, 85,170,
     85,238, 85,170,119,170, 85,170,
    221,170,119,170,221,170,119,170,
    221,170,255,170,221,170,255,170,
     85,255,170,255, 85,255,170,255,
    221,119,221,255,221,119,221,255,
    119,255,221,255,119,255,221,255,
    119,255,255,255,119,255,255,255,
    255,255,255,255,255,255,255,255  // 16
    };

// 8 pages, 8 bytes tall, 128 px wide
static uint8_t screen_buffer[(128 * 8)];

class Point {
    private:
        uint8_t x, y;

    public:
        Point(uint8_t x0, uint8_t y0){
            set(x0, y0);
        }

        void set(uint8_t x0, uint8_t y0){
            x = x0;
            y = y0;
        }

        uint8_t getx(){
            return x;
        }

        uint8_t gety(){
            return y;
        }        
};

static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    /* TODO why doesn't this work???
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 11,
        .scl_io_num = 10,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    */
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 11;
    conf.scl_io_num = 10;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void oled_init_1(void)
{
    uint8_t data[2];
    // Set MUX Ratio A8h, 3Fh 
    // BAM TODO: Something to do with frame refresh?
    data[0] = 0xA8;
    data[1] = 0x3F;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Display Offset D3h, 00h 
    // BAM TODO: Set veritical shift, what?  00 is set after reset, so this is just ensuring default value?
    data[0] = 0xD3;
    data[1] = 0x00;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Display Start Line 40h
    data[0] = 0x40;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Segment re-map A0h/A1h
    // A0 means column address 0 is mapped to SEG0 BAM TODO what???
    data[0] = 0xA0;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set COM Output Scan Direction C0h/C8h 
    // BAM: C0 is normal (default) COM0 -> COM[N-1], C8 is remapped, COM[N-1] -> COM0.  N is multiplex ratio
    data[0] = 0xC0;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set COM Pins hardware configuration DAh, 02
    // Alternative COM pin configuration, Enable COM Left/Right remap
    data[0] = 0xDA; 
    data[1] = 0x02;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Contrast Control 81h, 7Fh
    // Set [1-256] contrast steps.  Default is 7F
    data[0] = 0x81;
    data[1] = 0x7F;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Entire Display On A4h
    data[0] = 0xA4;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Normal Display A6h
    // BAM: As opposed to INVERSE display
    data[0] = 0xA6;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Osc Frequency D5h, 80h
    // Set Display Clock Divide Ratio/Oscillator Frequency
    // Lower 4 bits define divide ration D of the display clocks (DCLK) - default is 0000, but it's + 1 (cant have 0) so 1
    // Upper 4 bits set oscillator frequency fosc.  Default is 1000b 
    data[0] = 0xD5;
    data[1] = 0x80;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Enable charge pump regulator 8Dh, 14h
    // 14 enables charge pump during displayu on
    data[0] = 0x8D;
    data[1] = 0x14;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Display On AFh
    data[0] = 0xAF;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Normal Display A6h
    // BAM: As opposed to INVERSE display
    //data[0] = 0xA7;
    //ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Turn something on
}

void oled_init_2(void){
    uint8_t data[4];
    // Display Off
    data[0] = 0xAE;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Osc Frequency D5h, 80h
    // Set Display Clock Divide Ratio/Oscillator Frequency
    // Lower 4 bits define divide ration D of the display clocks (DCLK) - default is 0000, but it's + 1 (cant have 0) so 1
    // Upper 4 bits set oscillator frequency fosc.  Default is 1000b 
    data[0] = 0xD5;
    data[1] = 0x80;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set MUX Ratio A8h, 3Fh 
    // BAM TODO: Something to do with frame refresh?
    data[0] = 0xA8;
    data[1] = 0x3F;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Display Offset D3h, 00h 
    // BAM TODO: Set veritical shift, what?  00 is set after reset, so this is just ensuring default value?
    data[0] = 0xD3;
    data[1] = 0x00;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Display Start Line 40h
    data[0] = 0x40;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Enable charge pump regulator 8Dh, 14h
    // 14 enables charge pump during displayu on
    data[0] = 0x8D;
    data[1] = 0x14;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Memory Mode
    // 00 HORIZONTAL ADDRESSING MODE - Page addressing mode is 02 and default, but this follows adafruit
    data[0] = 0x20; 
    data[1] = 0x00;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Set Segment re-map A0h/A1h
    // A0 means column address 0 is mapped to SEG0 BAM TODO what???
    data[0] = 0xA1;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set COM Output Scan Direction C0h/C8h 
    // BAM: C0 is normal (default) COM0 -> COM[N-1], C8 is remapped, COM[N-1] -> COM0.  N is multiplex ratio
    data[0] = 0xC8;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set COM Pins hardware configuration DAh, 02
    data[0] = 0xDA; 
    data[1] = 0x12;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Contrast Control 81h, 7Fh
    // Set [1-256] contrast steps.  Default is 7F
    data[0] = 0x81;
    data[1] = 0xCF;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Pre-charge Period D9 F1 
    data[0] = 0xD9;
    data[1] = 0xF1;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Set VCOMH Deselect Level DB 40
    data[0] = 0xDB;
    data[1] = 0x40;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));        

    // Entire Display On (resume) A4h
    data[0] = 0xA4;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Normal Display A6h
    // BAM: As opposed to INVERSE display
    data[0] = 0xA6;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Deactivate Scroll
    data[0] = 0x2E;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Display On AFh
    data[0] = 0xAF;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Turn something on
    // WHITE is 1, BLACK is 0
    // Set Page Address 22 00 FF
    data[0] = 0x22;
    data[1] = 0x00;
    data[2] = 0x07;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Column Address 21 0 7 TODO 3rd arg is set to WIDTH - 1, that should be 128-1=127 => 0x7F
    data[0] = 0x21;
    data[1] = 0x00;
    data[2] = 0x7F;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    uint16_t count = 128 * ((64 + 7) / 8);
    while (count) {
        data[0] = 0x7F;
        data[1] = 0xFF;
        data[2] = 0xFF;
        ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
        count -= 2;
    }
}

void oled_init(void){
    uint8_t data[3];
    // Display Off
    data[0] = 0x00;
    data[1] = 0xAE;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Osc Frequency D5h, 80h
    // Set Display Clock Divide Ratio/Oscillator Frequency
    // Lower 4 bits define divide ration D of the display clocks (DCLK) - default is 0000, but it's + 1 (cant have 0) so 1
    // Upper 4 bits set oscillator frequency fosc.  Default is 1000b 
    data[0] = 0x00;
    data[1] = 0xD5;
    data[2] = 0x80;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set MUX Ratio A8h, 3Fh 
    // BAM TODO: Something to do with frame refresh?
    data[0] = 0x00;
    data[1] = 0xA8;
    data[2] = 0x3F;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Display Offset D3h, 00h 
    // BAM TODO: Set veritical shift, what?  00 is set after reset, so this is just ensuring default value?
    data[0] = 0x00;
    data[1] = 0xD3;
    data[2] = 0x00;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Display Start Line 40h
    data[0] = 0x00;
    data[1] = 0x40;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Enable charge pump regulator 8Dh, 14h
    // 14 enables charge pump during displayu on
    data[0] = 0x00;
    data[1] = 0x8D;
    data[2] = 0x14;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Memory Mode
    // 00 HORIZONTAL ADDRESSING MODE - Page addressing mode is 02 and default, but this follows adafruit
    data[0] = 0x00;
    data[1] = 0x20; 
    data[2] = 0x00;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Set Segment re-map A0h/A1h
    // A0 means column address 0 is mapped to SEG0 BAM TODO what???
    data[0] = 0x00;
    data[1] = 0xA1;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set COM Output Scan Direction C0h/C8h 
    // BAM: C0 is normal (default) COM0 -> COM[N-1], C8 is remapped, COM[N-1] -> COM0.  N is multiplex ratio
    data[0] = 0x00;
    data[1] = 0xC8;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set COM Pins hardware configuration DAh, 02
    data[0] = 0x00;
    data[1] = 0xDA; 
    data[2] = 0x12;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Contrast Control 81h, 7Fh
    // Set [1-256] contrast steps.  Default is 7F
    data[0] = 0x00;
    data[1] = 0x81;
    data[2] = 0xCF;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Pre-charge Period D9 F1 
    data[0] = 0x00;
    data[1] = 0xD9;
    data[2] = 0xF1;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Set VCOMH Deselect Level DB 40
    data[0] = 0x00;
    data[1] = 0xDB;
    data[2] = 0x40;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));        

    // Entire Display On (resume) A4h
    data[0] = 0x00;
    data[1] = 0xA4;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Normal Display A6h
    // BAM: As opposed to INVERSE display
    data[0] = 0x00;
    data[1] = 0xA6;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Deactivate Scroll
    data[0] = 0x00;
    data[1] = 0x2E;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));    

    // Display On AFh
    data[0] = 0x00;
    data[1] = 0xAF;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

}

uint16_t coord_to_frame(uint8_t x, uint8_t y){
    // The OLED screen is divieded into 8 pages stacked vertically,
    // each is 8 px tall and 128 px wide.  They are ordered top to bottom,
    // left to right within a page, then top to bottom by page
    // Coordinates are given (x increasing to right, y increasing to bottom)
    // Return a pixel index that corresponds to the frame ordering of the pixel indicated
    // by the coordinates.
    uint16_t page = y / 8;
    uint16_t page_row = y % 8;
    uint16_t column = x;
    return ((page * (128 * 8)) + (column * 8) + (page_row));
}

void write_px_to_buffer(uint16_t px, uint8_t* buffer){
    uint16_t byte_index = px / 8;
    uint8_t bit = px % 8;
    //printf("Writing px@ %d; byte: %d, bit: %d\n", px, byte_index, bit);
    buffer[byte_index] = buffer[byte_index] | (0x01 << bit);
}

void clear_buffer(uint8_t* buffer){
    for(int i = 0; i < (128 * 8); i++){
        buffer[i] = 0;
    }
}

void fill_buffer(uint8_t* buffer){
    for(int i = 0; i < (128 * 8); i++){
        buffer[i] = 255;
    }
}

void draw_buffer(uint8_t* buffer){
    uint8_t data[2];
    for(int i = 0; i < (128 * 8); i++){
        data[0] = 0x40;
        data[1] = screen_buffer[i];
        ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    }
}

void draw_buffer_slowly(uint8_t* buffer, uint8_t delay_ms){
    uint8_t data[2];
    for(int i = 0; i < (128 * 8); i++){
        data[0] = 0x40;
        data[1] = screen_buffer[i];
        ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
        vTaskDelay(delay_ms / portTICK_PERIOD_MS);
    }
}

void dump_buffer(uint8_t* buffer){
    for(int i = 0; i < (128 * 8); i++){
        printf("%d, ", buffer[i]);
        if ((i % 127) == 0) printf("\n===================\n");
    }
}

void oled_spiral(int size){

    // 8 pages, 8 bytes tall, 96 px wide
    static uint8_t screen_buffer[(96 * 8)];
    int x = 0;
    int y = 0;
    int last_x = -1;
    int last_y = 0;
    int filled = 0;
    int left = 0;
    int right = 95;
    int bottom = 63;
    int top = 0;

    while(filled < size){

        filled++;
        printf("print to buffer (%d, %d)\n", x, y);
        write_px_to_buffer(coord_to_frame(x, y), screen_buffer);

        // Get next pointer location
        if(last_x < x){
            last_x = x;
            x++;
        }
        else if (last_x > x){
            last_x = x;
            x--;
        }
        else if (last_y < y){
            last_y = y;
            y++;
        }
        else if (last_y > y){
            last_y = y;
            y--;
        }

        // Check we're not out of bounds
        if (x > right){ // Go down
            x--;
            y--;
        }
        if (x < left){ // Go up
            x++;
            y++;
        }
        if (y > top){
            y--;
            x++;
        }
        if (y < bottom){
            y++;
            x--;
        }
                    
    }

    uint8_t data[2];
    for(int i = 0; i < (96 * 8); i++){
        data[0] = 0x40;
        data[1] = screen_buffer[i];
        ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
    }
    
}

void draw_line(Point p0, Point p1){
    float slope;
    if(std::abs(p1.gety() - p0.gety()) < std::abs(p1.getx() - p0.getx())){ // Line is longer in horizontal direction
        float y = p0.gety();
        slope = ((float)(p1.gety() - p0.gety()) / (p1.getx() - p0.getx()));
        if(p0.getx() < p1.getx()){
            for(uint8_t x = p0.getx(); x < p1.getx(); x++){
                write_px_to_buffer(coord_to_frame(x, (uint8_t)y), screen_buffer);
                y += slope;
            }
        } else {
            for(uint8_t x = p0.getx(); x > p1.getx(); x--){
                write_px_to_buffer(coord_to_frame(x, (uint8_t)y), screen_buffer);
                y -= slope;            
            }
        }
    }
    else { // Line is 45 degrees or longer in vertical direction
        float x = p0.getx();
        slope = ((float)(p1.getx() - p0.getx()) / (p1.gety() - p0.gety()));        
        if(p0.gety() < p1.gety()){
            for(uint8_t y = p0.gety(); y < p1.gety(); y++){
                write_px_to_buffer(coord_to_frame((uint8_t)x, y), screen_buffer);
                x += slope;
            }
        } else {
            for(uint8_t y = p0.gety(); y > p1.gety(); y--){
                write_px_to_buffer(coord_to_frame((uint8_t)x, y), screen_buffer);
                x -= slope;
            }
        }
    }
}

void draw_line_to_fill_buffers(Point p0, Point p1, uint8_t* lbuf, uint8_t* rbuf){
    /*
    ASSUMPTION: Points P1, P2, P3 are given in clockwise order
    If line Y is increasing, we are on the left buffer (clockwise)
    If line Y is decreasing, we are on the right buffer (clockwise)
    if line Y is neither increasing nor decreasing
        min x of line to left buffer, max x of line to right buffer    
    */

   // TODO 
   // We have an issue drawing
   // Point(66, 0) -> Point(0,63)
   // The very first x coordinate (which should be 66, or is it reversed?) is not transferred
   // to the lbuffer
   // Q: is it reversed?
   // x range 66
   // y range 63, y is increasing, x is decreasing, line is longer in horizontal
    float slope;
    if(p0.gety() == p1.gety()) { // Simple case, min_x to lbuf and max_x to rbuf
        if(p0.getx() > p1.getx()) {
            rbuf[p0.gety()] = p0.getx();
            lbuf[p0.gety()] = p1.getx();
        } else {
            rbuf[p0.gety()] = p1.getx();
            lbuf[p0.gety()] = p0.getx();
        }
        return; // Early return, don't need to do the rest of this
    }

    // Normal case - Which buffer does this line go to?
    uint8_t* buffer;
    if(p0.gety() < p1.gety()) { // Y is increasing
        buffer = lbuf;
    } else { // Y is decreasing
        buffer = rbuf;
    }



    // TODO here begins duplicated code from draw_line
    if(std::abs(p1.gety() - p0.gety()) < std::abs(p1.getx() - p0.getx())){ // Line is longer in horizontal direction Multiple xs for same ys
        float y = p0.gety();
        slope = ((float)(p1.gety() - p0.gety()) / (p1.getx() - p0.getx()));


        // If lbuf and x increasing, reverse x direction
        // If rbuf and x decreasing, reverse x direction
        // This is because multiple xs can share the same ys, we want the widest possible x range for each y

        if(p0.getx() < p1.getx()){ // x increasing
            if(buffer == lbuf){
                y = p1.gety();
                for(uint8_t x = p1.getx(); x > p0.getx(); x--){
                    buffer[(uint8_t)y] = x;
                    y -= slope;
                }
            } else {
                for(uint8_t x = p0.getx(); x < p1.getx(); x++){
                    buffer[(uint8_t)y] = x;
                    y += slope;
                }
            }
        } else {                   // x decreasing
            if(buffer == rbuf){
                y = p1.gety();
                for(uint8_t x = p1.getx(); x < p0.getx(); x++){
                    buffer[(uint8_t)y] = x;
                    y += slope;            
                }
            } else {
                for(uint8_t x = p0.getx(); x > p1.getx(); x--){
                    buffer[(uint8_t)y] = x;
                    y -= slope;            
                }
            }
        }
    }
    else { // Line is 45 degrees or longer in vertical direction
        float x = p0.getx();
        slope = ((float)(p1.getx() - p0.getx()) / (p1.gety() - p0.gety()));        
        if(p0.gety() < p1.gety()){
            for(uint8_t y = p0.gety(); y < p1.gety(); y++){
                buffer[(uint8_t)y] = x;
                x += slope;
            }
        } else {
            for(uint8_t y = p0.gety(); y > p1.gety(); y--){
                buffer[(uint8_t)y] = x;
                x -= slope;
            }
        }
    }

    // As a final measure "dot" the endpoints.  This takes care of any sloppyness from float->int stuff
    buffer[p0.gety()] = p0.getx();
    buffer[p1.gety()] = p1.getx();
}


void print_buffers(uint8_t* lbuffer, uint8_t* rbuffer){
    printf("Y   LBUFFER RBUFFER\n");
    for(int i = 0; i < FRAME_Y_RESOLUTION; i++){
        printf("%d:  %d,  %d\n", i, lbuffer[i], rbuffer[i]);
    }
}

void fill_tri(Point p1, Point p2, Point p3, uint8_t* buffer){
    //printf("(%d, %d)\n", p1.getx(), p1.gety());
    //printf("(%d, %d)\n", p2.getx(), p2.gety());
    //printf("(%d, %d)\n", p3.getx(), p3.gety());
    //draw_line(p1, p2);
    //draw_line(p2, p3);
    //draw_line(p3, p1);

    /*
    Idea is to use two buffers, left and right, to hold the x coordinates
    of the left and right side of a horizontal line between them, at each
    vertical position that should be rendered.  Then just draw the lines.
    \<---------->\
     \<---------->\
      \<---------->|
       \<--------->|
        \<-------->|
    ASSUMPTION: Points P1, P2, P3 are given in clockwise order
    If line Y is increasing, we are on the left buffer (clockwise)
    If line Y is decreasing, we are on the right buffer (clockwise)
    if line Y is neither increasing nor decreasing
        min x of line to left buffer, max x of line to right buffer
    */
    uint8_t buf_left[FRAME_Y_RESOLUTION] = { }, buf_right[FRAME_Y_RESOLUTION] = { }; 
    draw_line_to_fill_buffers(p1, p2, buf_left, buf_right);
    print_buffers(buf_left, buf_right);
    draw_line_to_fill_buffers(p2, p3, buf_left, buf_right);
    print_buffers(buf_left, buf_right);
    draw_line_to_fill_buffers(p3, p1, buf_left, buf_right);
    print_buffers(buf_left, buf_right);
    for(int i = 0; i < FRAME_Y_RESOLUTION; i++){
        if(buf_left[i] != buf_right[i]){
            draw_line(Point(buf_left[i], i), Point(buf_right[i], i));
        }
    }


}

void shade_px(uint8_t* buffer, uint8_t shade, uint8_t x, uint8_t y){
    if (shade >= MAX_SHADES) shade = MAX_SHADES - 1; // TODO this is clumsy, fix MAX SHADES so we don't need the -1
    uint8_t shade_x = x % 8;
    uint8_t shade_y = y % 8;
    bool value = ((shade8x8[((shade * 8) + shade_x)]) >> shade_y) & 0x01;

    // Write the pixel to the buffer if we should, else leave it blank
    if (value) write_px_to_buffer(coord_to_frame(x, y), buffer);
}

void shade_demo(uint8_t* buffer){
    // Shade left to right
    uint8_t shade;
    for (int x = 0; x < 128; x++){
        for (int y = 0; y < (8 * 8); y++){
            shade = (x * (MAX_SHADES - 1)) / 128; // 128 is max x value
            //printf("Shading (%d, %d) with %d value...\n", x, y, shade);
            shade_px(buffer, shade, x, y);
        }
    }
}

void app_main(void)
{
    printf("Hello world!\n");

    printf("Setting up i2c!\n");
    uint8_t data[4];
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");


    // Set the SSD1306 internal command registers as per
    // https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf

    // Initialize the OLED Screen
    oled_init();

    // Turn something on
    // WHITE is 1, BLACK is 0
    // Set Page Address 22 00 FF
    data[0] = 0x00;
    data[1] = 0x22;
    data[2] = 0x00;
    data[3] = 0x07;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 4, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Set Column Address 21 0 7 TODO 3rd arg is set to WIDTH - 1, that should be 128-1=127 => 0x7F
    data[0] = 0x00;
    data[1] = 0x21;
    data[2] = 0x00;
    data[3] = 0x7F;
    ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 4, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));

    // Demo draw tri
    clear_buffer(screen_buffer);
    //fill_tri(Point(0,0), Point(127, 0), Point(66, 63), screen_buffer);
    fill_tri(Point(0,63), Point(127, 63), Point(66, 0), screen_buffer);    
    //fill_tri(Point(0,0), Point(0, 63), Point(127, 31), screen_buffer);
    //fill_tri(Point(127,0), Point(127, 63), Point(0, 31), screen_buffer);   
    draw_buffer(screen_buffer);

    /* // demo draw lines
    clear_buffer(screen_buffer);
    draw_line(0,0, 127, 63);
    draw_line(0,63, 127, 0);
    draw_buffer(screen_buffer);
    draw_line(  0,  0, 127,  0);
    draw_line(127,  0, 127, 63);
    draw_line(127, 63,   0, 63);
    draw_line(  0, 63,   0,  0);
    draw_buffer(screen_buffer);
    */
    
    /* // Fill screen one horizontal line at a time
    for (int y = 0; y < 64; y++){
        //clear_buffer(screen_buffer);
        draw_line(0, y,  127, y);
        draw_buffer(screen_buffer);
    }
    */

    /* // Wipe screen one column at a time
    clear_buffer(screen_buffer);
    draw_buffer(screen_buffer);
    fill_buffer(screen_buffer);
    draw_buffer_slowly(screen_buffer, 10);
     */

    //clear_buffer(screen_buffer);
    //draw_buffer(screen_buffer);
    //draw_line(0, 0,  127, 0);
    //dump_buffer(screen_buffer);

    /* // Shade demo
    clear_buffer(screen_buffer);
    draw_buffer(screen_buffer);
    shade_demo(screen_buffer);
    draw_buffer(screen_buffer);
    */

    /* 

    for (int i = 0; i < 50; i++){
        uint16_t count = 128 * ((64 + 7) / 8);
        uint8_t b = 0x01;
        while (count) {
            data[0] = 0x40;
            data[1] = b;
            if (i % 2 == 0){
                b = b << 1 | b >> 7; // Rotate left
            }
            else {
                b = b >> 1 | b << 7; // Rotate right
            }
            ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
            count -= 1;
        }
    }

    */




    vTaskDelay(2000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C unitialized successfully");

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
