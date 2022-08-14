#ifndef FEEBEE_CAM__RTC_H
#define FEEBEE_CAM__RTC_H

#include <I2C_BM8563.h>

namespace FeebeeCam {

    extern I2C_BM8563 rtc;

    bool initializeRTC(bool override = false);
    bool isRTCSetup();
    void displayNow();
    
}

#endif
