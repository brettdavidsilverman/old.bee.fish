#include "i2c.h"

namespace FeebeeCam {

    TwoWire wire(0);
    DFRobot_I2CMultiplexer multiplexer(0x70, &wire);

    void initializeI2C() {
        wire.begin(SDA, SCL);
        // Print I2C device of each port
        // There are 8 ports, but the last one is
        // reserved for neo pixels lighting (not compatible)
        for (uint8_t port = 0; port < 7; ++port) {
            Serial.print("Port:");
            Serial.print(port);
            uint8_t* dev = multiplexer.scan(port);
            while (*dev) {
                Serial.print(" i2c addr ");
                Serial.print(*dev, HEX);
                dev++;
            }
            Serial.println();
        }


    }
}


