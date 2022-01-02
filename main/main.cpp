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

extern "C" {
    void app_main();
}

// 8 pages, 8 bytes tall, 96 px wide
    static uint8_t screen_buffer[(128 * 8)];

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
    // each is 8 px tall and 96 px wide.  They are ordered top to bottom,
    // left to right within a page, then top to bottom by page
    // Coordinates are given (x increasing to right, y increasing to bottom)
    // Return a pixel index that corresponds to the frame ordering of the pixel indicated
    // by the coordinates.
    uint8_t page = y / 8;
    uint8_t page_row = y % 8;
    uint8_t column = x;
    return ((page * (96 * 8)) + (column * 8) + (page_row));
}

void write_px_to_buffer(uint16_t px, uint8_t* buffer){
    buffer[(px / 8)] = buffer[px / 8] | (px % 8);
}

void clear_buffer(uint8_t* bufer){
    for(int i = 0; i < (96 * 8); i++){
        bufer[i] = 0;
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

void draw_line(){


    // Draw 25 px wide line at row 2
    for(uint8_t i = 0; i < 64; i++){
        write_px_to_buffer(coord_to_frame(i, 2), screen_buffer);
    }
    
    uint8_t data[2];
    for(int i = 0; i < (128 * 8); i++){
        data[0] = 0x40;
        data[1] = screen_buffer[i];
        ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS));
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

    //for (int i = 0; i < 128; i++){
    //    oled_spiral(i);
    //}

    
    draw_line();

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
