#pragma once
#include <Wire.h>
#include <DFRobot_I2CMultiplexer.h>

namespace FeebeeCam {

    extern TwoWire wire;
    extern DFRobot_I2CMultiplexer multiplexer;

    void initializeI2C();
}
