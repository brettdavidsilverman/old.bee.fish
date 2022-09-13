#include <iostream>
#include "multiplexer.h"


namespace FeebeeCam {

    Adafruit_MCP23008 _multiplexer;
    TwoWire* multiplexerTwoWire = nullptr;
    bool initialized = false;

    bool deinitializeMultiplexer() {
        multiplexerTwoWire->end();
        return true;
    }

    bool initializeMultiplexer() {

        using namespace std;

        multiplexerTwoWire = &Wire;

        deinitializeMultiplexer();

        multiplexerTwoWire->setPins(SDA, SCL);
        initialized = multiplexerTwoWire->begin();

        if (!initialized) {
            cerr << "Error starting Wire 0" << endl;
            return false;
        }

        if (_multiplexer.begin(0x20, multiplexerTwoWire)) {
            return true;
        }
        else {
            Serial.println("Multiplexer not found");
            return false;
        }
    }
}
