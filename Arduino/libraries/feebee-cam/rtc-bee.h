#ifndef FEEBEE_CAM__RTC_H
#define FEEBEE_CAM__RTC_H

#include <I2C_BM8563.h>
#include <bee-fish.h>

namespace FeebeeCam {

    extern I2C_BM8563* rtc;

    bool initializeRTC(bool override = false);
//    bool isRTCSetup();
    void displayNow();
    BeeFishBString::BString getTime();
    BeeFishBString::BString getDate();
    BeeFishBString::BString getDateTime();
}

#endif
