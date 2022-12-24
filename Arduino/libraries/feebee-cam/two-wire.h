#ifndef FEEBEE_CAM_MULTIPLEXER
#define FEEBEE_CAM_MULTIPLEXER

#include <Wire.h>

namespace FeebeeCam {

    extern TwoWire* twoWire;

    bool initializeTwoWire(uint8_t i2cNum, uint8_t sda, uint8_t scl);
}

#endif