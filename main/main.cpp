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
#include <string>

#include "LomontShape.h"
#include "Graphics/Font.h"
//#include "3d_stuff.h" TODO guards for includes....
#include "demos.h"
#include "3d_stuff.h"

static const char *TAG = "esp-oled";

extern "C" {
    void app_main();
}


void app_main(void)
{
    printf("Hello world!\n");

    printf("Setting up i2c!\n");
    ESP_ERROR_CHECK(i2c_master_init());
    ESP_LOGI(TAG, "I2C initialized successfully");

    // Initialize the OLED Screen
    oled_init();


    //demo_text_hello_world();
    //demo_poles(screen_buffer);
    demo_lomont_shapes(screen_buffer);
        
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
