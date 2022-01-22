#pragma once
#include <stdint.h>

#define CHIP_FEATURE_EMB_FLASH  (1UL << 0)
#define CHIP_FEATURE_BT         (1UL << 4)
#define CHIP_FEATURE_BLE        (1UL << 5)

typedef struct {
    uint32_t features;       //!< bit mask of CHIP_FEATURE_x feature flags
    uint8_t cores;           //!< number of CPU cores
    uint8_t revision;        //!< chip revision number
} esp_chip_info_t;

void esp_restart(void);
void esp_chip_info(esp_chip_info_t* out_info);

typedef int esp_err_t;
typedef int i2c_port_t; 
typedef int i2c_mode_t;
typedef int TickType_t;

typedef struct{
    int mode;     /*!< I2C mode */
    int sda_io_num;      /*!< GPIO number for I2C sda signal */
    int scl_io_num;      /*!< GPIO number for I2C scl signal */
    bool sda_pullup_en;  /*!< Internal GPIO pull mode for I2C sda signal*/
    bool scl_pullup_en;  /*!< Internal GPIO pull mode for I2C scl signal*/

    union {
        struct {
            int clk_speed;      /*!< I2C clock frequency for master mode, (no higher than 1MHz for now) */
        } master;                    /*!< I2C master config */
        struct {
            int addr_10bit_en;   /*!< I2C 10bit address mode enable for slave mode */
            int slave_addr;     /*!< I2C address for slave mode */
            int maximum_speed;  /*!< I2C expected clock speed from SCL. */
        } slave;                     /*!< I2C slave config */
    };
    int clk_flags;              /*!< Bitwise of ``I2C_SCLK_SRC_FLAG_**FOR_DFS**`` for clk source choice*/
} i2c_config_t;

#define I2C_MODE_MASTER 0
#define GPIO_PULLUP_ENABLE 1
#define portTICK_RATE_MS 3


esp_err_t i2c_param_config(i2c_port_t i2c_num, const i2c_config_t *i2c_conf){
    printf("Called i2c_param_config\n");
    return 0;
} 

esp_err_t i2c_driver_install(i2c_port_t i2c_num, i2c_mode_t mode, size_t slv_rx_buf_len, size_t slv_tx_buf_len,
                             int intr_alloc_flags){
    printf("Called i2c_driver_install\n");
    return 0;
}

esp_err_t i2c_master_write_to_device(i2c_port_t i2c_num, uint8_t device_address,
                                     const uint8_t* write_buffer, size_t write_size,
                                     TickType_t ticks_to_wait)
{
    printf("Called i2c_master_write_to_device\n");
    return 0;
}

#define ESP_ERROR_CHECK(x) do {                                         \
        esp_err_t err_rc_ = (x);                                        \
    } while(0)
