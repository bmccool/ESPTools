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
#include "spi_flash_mmap.h"
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
#include "i2c_sniffer.h"

#include "driver/gpio.h"

#define I2C_IDLE 0
//#define I2C_START 1
#define I2C_TRX 2

#define GPIO_INPUT_SCL static_cast<gpio_num_t>(35)
#define GPIO_INPUT_SCL_PIN_SEL  (1ULL<<GPIO_INPUT_SCL)
#define GPIO_INPUT_SDA static_cast<gpio_num_t>(36)
#define GPIO_INPUT_SDA_PIN_SEL  (1ULL<<GPIO_INPUT_SDA)
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_SCL) | (1ULL<<GPIO_INPUT_SDA))

//#define GPIO_OUTPUT_SCL static_cast<gpio_num_t>(10)
//#define GPIO_OUTPUT_SDA static_cast<gpio_num_t>(11)
//#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_SCL) | (1ULL<<GPIO_OUTPUT_SDA))

#define ESP_INTR_FLAG_DEFAULT 0

static volatile uint8_t i2cStatus = I2C_IDLE;//Status of the I2C BUS
static uint32_t lastStartMillis = 0;//stoe the last time
static volatile uint8_t dataBuffer[9600];//Array for storing data of the I2C communication
static volatile uint16_t bufferPoiW=0;//points to the first empty position in the dataBufer to write
static uint16_t bufferPoiR=0;//points to the position where to start read from
static volatile uint8_t bitCount = 0;//counter of bit appeared on the BUS
static volatile uint16_t byteCount =0;//counter of bytes were writen in one communication.
static volatile uint8_t i2cBitD =0;//Container of the actual SDA bit
static volatile uint8_t i2cClk =0;//Container of the actual SCL bit
static volatile uint8_t i2cAck =0;//Container of the last ACK value
static volatile uint8_t i2cCase =0;//Container of the last ACK value
static volatile uint16_t falseStart = 0;//Counter of false start events
//static volatile uint8_t respCount =0;//Auxiliary variable to help detect next byte instead of STOP
//these variables just for statistic reasons
static volatile uint16_t sclUpCnt = 0;//Auxiliary variable to count rising SCL
static volatile uint16_t sdaUpCnt = 0;//Auxiliary variable to count rising SDA
static volatile uint16_t sdaDownCnt = 0;//Auxiliary variable to count falling SDA

static QueueHandle_t gpio_evt_queue = NULL;


static void IRAM_ATTR i2cTriggerOnRiseSCL(void* arg)
{
    if(i2cBitD == 1){
        dataBuffer[bufferPoiW++] = '1';
    }
    else {
        dataBuffer[bufferPoiW++] = '0';
    }
}//END of i2cTriggerOnRaisingSCL() 

static void IRAM_ATTR i2cTriggerOnChangeSDA(void* arg)
{

	i2cBitD =  gpio_get_level(GPIO_INPUT_SDA);

	//if(i2cBitD == 1) // SDA Rising
	//{
    //    dataBuffer[bufferPoiW++] = '1';
	//}
	//else // SDA Falling
	//{
    //    dataBuffer[bufferPoiW++] = '0';
	//}
}//END of i2cTriggerOnChangeSDA()

void processDataBuffer()
{
	if(bufferPoiW == bufferPoiR)//There is nothing to say
		return;

	uint16_t pw = bufferPoiW;

	//print out the content of the buffer
    printf("\n");
	for(int i=bufferPoiR; i< pw; i++)
	{
        
		printf("%c", static_cast<char>(dataBuffer[i]));
		bufferPoiR++;		
	}
	
	//if there is no I2C action in progress and there wasn't during the Serial.print then buffer was printed out completly and can be reset.
	if(i2cStatus == I2C_IDLE && pw==bufferPoiW)
	{
		bufferPoiW =0;
		bufferPoiR =0;
	}	
}//END of processDataBuffer()

static void gpio_i2c_sniffer(void* arg)
{
    uint32_t io_num;
    for (;;) {
        if(i2cStatus == I2C_IDLE){
            processDataBuffer();
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
        //if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
        //    printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(static_cast<gpio_num_t>(io_num)));
        //}
    }
}


static const char *TAG = "esp-oled";

extern "C" {
    void app_main();
}



void resetI2cVariables(){
    i2cStatus = I2C_IDLE;
    bufferPoiW = 0;
    bufferPoiR = 0;
    bitCount = 0;
    byteCount = 0;
    falseStart = 0;
}


void app_main(void)
{
    // Configure Output Pins
    ////zero-initialize the config structure.
    //gpio_config_t io_conf = {};
    ////disable interrupt
    //io_conf.intr_type = GPIO_INTR_DISABLE;
    ////set as output mode
    //io_conf.mode = GPIO_MODE_OUTPUT;
    ////bit mask of the pins that you want to set,e.g.GPIO18/19
    //io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    ////disable pull-down mode
    //io_conf.pull_down_en = static_cast<gpio_pulldown_t>(0);
    ////disable pull-up mode
    //io_conf.pull_up_en = static_cast<gpio_pullup_t>(0);
    ////configure GPIO with the given settings
    //gpio_config(&io_conf);



    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    
    // Configure the Input Pins
    // SCL triggers on rising edge
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = static_cast<gpio_pullup_t>(1);
    gpio_config(&io_conf);

    // SDA triggers on any change
    gpio_set_intr_type(GPIO_INPUT_SDA, GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    //gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_i2c_sniffer, "gpio_i2c_sniffer", 2048, NULL, 10, NULL);


    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_SCL, i2cTriggerOnRiseSCL, (void*) GPIO_INPUT_SCL);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_SDA, i2cTriggerOnChangeSDA, (void*) GPIO_INPUT_SDA);

	printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    i2c_master_init();

    int cnt = 0;
    uint8_t data[3];
    data[0] = 0xFF;
    data[1] = 0xFF;
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        data[2] = static_cast<uint8_t>(cnt++);
        printf("\nWriting %d %d %d\n", static_cast<uint8_t>(data[0]), static_cast<uint8_t>(data[1]), static_cast<uint8_t>(data[2]));
        i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 3, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    }
}
