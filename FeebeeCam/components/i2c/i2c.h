#pragma once

#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO           13      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           4      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              1                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000 //400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   1                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   1                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

class I2CMaster {
private: 
    static bool _i2cMasterInitialized;
public:
    I2CMaster(int port, uint8_t address);

    bool i2CMasterInitialized();


    virtual esp_err_t readRegister(uint8_t reg, uint8_t *data, size_t length = 1);
    virtual esp_err_t writeRegister(uint8_t reg, uint8_t data);
    virtual i2c_port_t port() { return _port; }
    virtual uint8_t address() { return _address; }

protected:
    i2c_port_t _port;    /**< I2C_NUM_0 or I2C_NUM_1 */
    uint8_t _address;    /**< Hardware address of the slave device */

};
