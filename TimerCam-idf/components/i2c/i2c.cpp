#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "../error/error.h"
#include "i2c.h"
 
#define ACK_CHECK_EN 0x1            /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0           /*!< I2C master will not check ack from slave */
#define ACK_VAL i2c_ack_type_t(0x0)                 /*!< I2C ack value */
#define NACK_VAL 0x1                /*!< I2C nack value */

static const char *TAG = "i2c-init";

I2CMaster::I2CMaster(int port, uint8_t address) {

    if (port < 0)
        port = I2C_MASTER_NUM;

    _port = (i2c_port_t)port;
    _address = address;

    if (_i2cMasterInitialized)
        return;

    ESP_LOGI(TAG,"Initializing i2c-master on port %i", _port);

    _i2cMasterInitialized = false;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = (gpio_num_t) I2C_MASTER_SDA_IO,
        .scl_io_num = (gpio_num_t) I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = I2C_MASTER_FREQ_HZ
        }
    };

    i2c_param_config(_port, &conf);

    esp_err_t res = i2c_driver_install(_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    CHECK_ERROR(res, TAG, "Initializing I2C Master")

    _i2cMasterInitialized = true;


}

bool I2CMaster::i2CMasterInitialized() {
    return _i2cMasterInitialized;
}

esp_err_t I2CMaster::readRegister(uint8_t reg, uint8_t *data, size_t length) {

    ESP_LOGI(TAG,"Reading register %u on port %i and address %#x", reg, _port, _address);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(_port, cmd, 1000/portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    CHECK_ERROR(ret, TAG, "Error reading register %u cmd begin", reg);

    //vTaskDelay(30/portTICK_RATE_MS);

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
    i2c_master_read(cmd, data, length, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    ret = i2c_master_cmd_begin(_port, cmd, 1000/portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    CHECK_ERROR(ret, TAG, "Error reading register %u with wait of %u", reg, 1000/portTICK_RATE_MS);

    return ESP_OK;
}

esp_err_t I2CMaster::writeRegister(uint8_t reg, uint8_t d) {

    ESP_LOGI(TAG,"Write register port %i address %#x register %#x", _port, _address, reg);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (_address << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, d, ACK_CHECK_EN);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(_port, cmd, 1000/portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    CHECK_ERROR(ret, TAG, "Error writing to register port %i address %#x register %#x", _port, _address, reg);

    return ESP_OK;
}

bool I2CMaster::_i2cMasterInitialized = false;