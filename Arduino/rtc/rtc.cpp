#include <iostream>
#include "rtc.h"
#include "local-time.h"

#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14


namespace FeebeeCam {
    
    BM8563 RTC;

    bool initializeRTC() {

        std::cerr  << "Initializing RTC" << std::endl;

        Wire1.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);

        RTC.begin(&Wire1);

        time_t now;            // this is the epoch
        std::tm localTime; // the structure tm holds time information in a more convenient way
        time(&now);                              // read the current time
        localtime_r(&now, &localTime); // update the structure tm with the current time

        rtc_time_type rtcTime;
        rtc_date_type rtcDate;

        rtcTime.Hours = localTime.tm_hour;
        rtcTime.Minutes = localTime.tm_min;
        rtcTime.Seconds = localTime.tm_sec;

        rtcDate.WeekDay = localTime.tm_wday;
        rtcDate.Month = localTime.tm_mon + 1;
        rtcDate.Date = localTime.tm_mday;
        rtcDate.Year = localTime.tm_year + 1900;

        RTC.setTime(&rtcTime);
        RTC.setDate(&rtcDate);

        return true;
    }

    bool isRTCInitialized() {
        return false;
    }

}
