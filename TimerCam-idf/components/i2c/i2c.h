#pragma once

#define I2C_MASTER_SCL_IO           13      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           4      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              1                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          100000 //400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   1                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   1                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

class I2CMaster {
private: 
    inline static bool _i2cMasterInitialized = false;
public:
    I2CMaster(int port = -1);

    bool i2CMasterInitialized();
};
