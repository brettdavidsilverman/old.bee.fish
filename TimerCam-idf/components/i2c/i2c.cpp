#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "driver/i2c.h"
#include "error.h"
#include "i2c.h"
 

static const char *TAG = "i2c-init";

I2CMaster::I2CMaster(int port) {

    if (_i2cMasterInitialized)
        return;

    _i2cMasterInitialized = false;

    if (port < 0)
        port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = I2C_MASTER_FREQ_HZ
        }
    };

    i2c_param_config(port, &conf);

    esp_err_t res = i2c_driver_install(port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);

    CHECK_ERROR(res, TAG, "Initializing I2C Master")

    _i2cMasterInitialized = true;

}

 bool I2CMaster::i2CMasterInitialized() {
    return _i2cMasterInitialized;
}


