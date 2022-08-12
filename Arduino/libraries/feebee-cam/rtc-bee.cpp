#include <iostream>
#include <sys/time.h>
#include "rtc-bee.h"
#include "local-time.h"
#include "setup.h"

#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14

namespace FeebeeCam {
    
    BM8563 RTC;

    bool initializeRTC(bool forceUpdate) {

        std::cerr  << "Initializing RTC" << std::endl;

        static TwoWire MyWire(0);
        static bool initialized = false;

        if (initialized)
            MyWire.end();

        MyWire.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);
        RTC.begin(&MyWire);
        initialized = true;

        if (FeebeeCam::isRTCSetup() && !forceUpdate) {
            std::cerr << "RTC has been set. No need to get internet time" << std::endl;
            std::cerr << "Setting system time based off of RTC" << std::endl;

            std::cerr << "Setting timezone" << std::endl;

            setenv("TZ", MY_TIMEZONE, 1);          // Set environment variable with your time zone
            tzset();

            std::tm localTime{};

            rtc_time_type rtcTime;
            rtc_date_type rtcDate;
            RTC.getTime(&rtcTime);
            RTC.getDate(&rtcDate);

            localTime.tm_year = rtcDate.Year - 1900;
            localTime.tm_mon  = rtcDate.Month - 1;
            localTime.tm_mday = rtcDate.Date;
            localTime.tm_wday = rtcDate.WeekDay;
            localTime.tm_hour = rtcTime.Hours;
            localTime.tm_min  = rtcTime.Minutes;
            localTime.tm_sec  = rtcTime.Seconds;

            // Convert the local time to epoch
            time_t now = mktime(&localTime);

            if (now > 0) {
                // Set the system time
                struct timeval timeValue {};
                timeValue.tv_sec = now;
                int ret = settimeofday(&timeValue, NULL);
                if (ret == 0) {
                    FeebeeCam::displayNow();
                    return true;
                }
                else {
                    std::cerr << "settimeofday failed with return value of " << ret << std::endl;
                }
            }
            else {
                std::cerr << "mktime failed" << std::endl;
            }

        }
        
        if (!FeebeeCam::initializeTime())
            return false;

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

        FeebeeCam::_setup->_isRTCSetup = true;
        FeebeeCam::_setup->save();

        FeebeeCam::displayNow();

        return true;
    }

    bool isRTCSetup() {
        return FeebeeCam::_setup->_isRTCSetup;
    }

    void displayNow() {

        time_t now = time(NULL);
        std::tm localTime; // the structure tm holds time information in a more convenient way
        localtime_r(&now, &localTime); // update the structure tm with the current time
        

        rtc_time_type rtcTime;
        rtc_date_type rtcDate;

        RTC.getTime(&rtcTime);
        RTC.getDate(&rtcDate);

        std::cerr << "System clock Date: " << localTime.tm_year + 1900 << "/" << localTime.tm_mon + 1 << "/" << localTime.tm_mday << std::endl;
        std::cerr << "System clock Time: " << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << std::endl;

        std::cerr << "RTC Date:          " << rtcDate.Year << "/" << (int)rtcDate.Month << "/" << (int)rtcDate.Date << std::endl;
        std::cerr << "RTC clock Time:    " << (int)rtcTime.Hours << ":" << (int)rtcTime.Minutes << ":" << (int)rtcTime.Seconds << std::endl;
    }

}
