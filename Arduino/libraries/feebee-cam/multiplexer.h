#pragma once
#include <Wire.h>
#include "Adafruit_MCP23008.h"

namespace FeebeeCam {

    extern Adafruit_MCP23008 _multiplexer;

    bool initializeMultiplexer();
}