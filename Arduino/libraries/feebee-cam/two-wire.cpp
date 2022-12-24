#include <iostream>
#include <Arduino.h>
#include "two-wire.h"

namespace FeebeeCam {

    TwoWire* twoWire = nullptr;

    bool twoWireInitialized = false;

    bool initializeTwoWire() {

        if (!twoWireInitialized)
            Wire.setPins(SDA, SCL);

        Wire.begin();

        twoWire = &Wire;

        twoWireInitialized = true;
        
        return true;

    }
}
