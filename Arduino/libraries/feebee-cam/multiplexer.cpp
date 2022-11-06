#include <iostream>
#include "multiplexer.h"


namespace FeebeeCam {

    Adafruit_MCP23008 _multiplexer;
    TwoWire* mainBoardTwoWire = nullptr;
    bool initialized = false;

    bool deinitializeMainBoardTwoWire() {
        mainBoardTwoWire->end();
        return true;
    }

    bool initializeMainBoardTwoWire() {

        using namespace std;

        mainBoardTwoWire = &Wire;

        deinitializeMainBoardTwoWire();

        mainBoardTwoWire->setPins(SDA, SCL);
        initialized = mainBoardTwoWire->begin();

        if (!initialized) {
            cerr << "Error starting Wire 0" << endl;
            return false;
        }

        if (_multiplexer.begin(0x20, mainBoardTwoWire)) {
            return true;
        }
        else {
            Serial.println("Multiplexer not found");
            return false;
        }
    }
}
