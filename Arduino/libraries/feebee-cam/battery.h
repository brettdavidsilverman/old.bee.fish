#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

namespace FeebeeCam {
    bool initializeBattery();

    // Enable battery power supply if USB is unplugged 
    void holdBatteryOutput();

    // Disable battery power supply if USB is unplugged 
    void disableBatteryOutput();

    // return: xxxx mv
    float getBatteryVoltage();
}

#ifdef __cplusplus
}
#endif