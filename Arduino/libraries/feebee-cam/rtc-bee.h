#ifndef FEEBEE_CAM__RTC_H
#define FEEBEE_CAM__RTC_H

#include "M5_BM8563.h"

namespace FeebeeCam {

    extern BM8563 RTC;

    bool initializeRTC(bool override = false);
    bool isRTCSetup();
    void displayNow();
    
}

#endif
