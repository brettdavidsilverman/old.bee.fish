#include "multiplexer.h"


namespace FeebeeCam {

    Adafruit_MCP23008 _multiplexer;
    TwoWire* multiplexerTwoWire = nullptr;

    bool initializeMultiplexer() {
        
        static bool initialized = false;

        if (initialized) {
            multiplexerTwoWire->end();
            delete multiplexerTwoWire;
            initialized = false;
        }

        multiplexerTwoWire = new TwoWire(0);
        multiplexerTwoWire->setPins(SDA, SCL);

        initialized = true;


        if (_multiplexer.begin(0x20, multiplexerTwoWire)) {
            Serial.println("Multiplexer initialized");
            return true;
        }
        else {
            Serial.println("Multiplexer not found");
            return false;
        }
    }
}
