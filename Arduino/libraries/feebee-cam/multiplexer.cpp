#include "multiplexer.h"


namespace FeebeeCam {
    Adafruit_MCP23008 _multiplexer;

    bool initializeMultiplexer() {
        if (_multiplexer.begin()) {
            Serial.println("Multiplexer initialized");
            return true;
        }
        else {
            Serial.println("Multiplexer not found");
            return false;
        }
    }
}
