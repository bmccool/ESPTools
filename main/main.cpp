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
#include <vector> // std::vector
#include <iostream> // cin, cout
#include <chrono> // function timing/metrics

#include "LomontShape.h"
//#include "3d_stuff.h"
#include "demos.h"

static const char *TAG = "esp-oled";

extern "C" {
    void app_main();
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
    /*
    uint8_t shade = 0;
    uint8_t direction = 1;
    while(true){
        clear_buffer(screen_buffer);
        //fill_tri(Point(0,0), Point(127, 0), Point(66, 63), screen_buffer);
        fill_tri(Point(0,63), Point(127, 63), Point(66, 0), screen_buffer, shade);    
        //fill_tri(Point(0,0), Point(0, 63), Point(127, 31), screen_buffer);
        //fill_tri(Point(127,0), Point(127, 63), Point(0, 31), screen_buffer);   
        draw_buffer(screen_buffer);
        shade += direction;
        if(shade == SHADE_SOLID) direction = -1;
        if(shade == SHADE_EMPTY) direction = 1;
    }
    */

    //cube_demo();
    //demo_rotate_box_z();
    //demo_rotate_cube(screen_buffer);
    //demo_rotate_shaded_cube(screen_buffer);
    //demo_rotate_shaded_cubes(screen_buffer);
    //demo_seizure_warning(screen_buffer);
    demo_lomont_shapes(screen_buffer);

    //clear_buffer(screen_buffer);
    //draw_buffer(screen_buffer);
    //Line l(Point3(64, 32, 0), Point3(64, -32, 0));
    //l.draw_to(screen_buffer);
    //draw_buffer(screen_buffer);
    

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
