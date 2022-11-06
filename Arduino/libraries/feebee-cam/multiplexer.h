#ifndef FEEBEE_CAM_MULTIPLEXER
#define FEEBEE_CAM_MULTIPLEXER

#include <Wire.h>
#include "Adafruit_MCP23008.h"

namespace FeebeeCam {

    extern TwoWire* mainBoardTwoWire;
    extern Adafruit_MCP23008 _multiplexer;

    bool initializeMainBoardTwoWire();
    bool deinitializeMainBoardTwoWire();
}

#endif