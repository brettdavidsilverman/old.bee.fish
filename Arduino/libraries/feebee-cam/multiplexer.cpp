#include "multiplexer.h"


namespace FeebeeCam {
    Adafruit_MCP23008 _multiplexer;

    bool initializeMultiplexer() {
        
        static TwoWire MyWire(2);

        MyWire.setPins(SDA, SCL);

        if (_multiplexer.begin(0x20, &MyWire)) {
            Serial.println("Multiplexer initialized");
            return true;
        }
        else {
            Serial.println("Multiplexer not found");
            return false;
        }
    }
}
