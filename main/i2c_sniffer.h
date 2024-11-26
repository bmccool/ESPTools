
void sniff_i2c(void){
    uint8_t data[2];
    data[0] = 0x01;
    data[1] = 0x23;

    while(true){
        ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, SSD1306_I2C_ADDR, data, 2, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    }
    
}