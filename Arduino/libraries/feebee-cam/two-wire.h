#ifndef FEEBEE_CAM_MULTIPLEXER
#define FEEBEE_CAM_MULTIPLEXER

#include <Wire.h>

namespace FeebeeCam {

    extern TwoWire* twoWire;

    bool initializeTwoWire();
}

#endif