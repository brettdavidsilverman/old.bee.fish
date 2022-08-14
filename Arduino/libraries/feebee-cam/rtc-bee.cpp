#include <iostream>
#include <sys/time.h>
#include "rtc-bee.h"
#include "local-time.h"
#include "setup.h"
#include "config.h"

#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14

namespace FeebeeCam {

    TwoWire MyWire(0);
    I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, MyWire);

    bool initializeRTC(bool forceUpdate) {

        std::cerr  << "Initializing RTC" << std::endl;

        static bool initialized = false;

        if (initialized)
            MyWire.end();

        MyWire.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);
        rtc.begin();
        initialized = true;

        if (FeebeeCam::isRTCSetup() && !forceUpdate) {
            std::cerr << "RTC has been set. No need to get internet time" << std::endl;
            std::cerr << "Setting system time based off of RTC" << std::endl;

            std::cerr << "Setting timezone" << std::endl;


            std::tm localTime{};

            I2C_BM8563_TimeTypeDef rtcTime;
            I2C_BM8563_DateTypeDef rtcDate;
            rtc.getTime(&rtcTime);
            rtc.getDate(&rtcDate);

            localTime.tm_year = rtcDate.year - 1900;
            localTime.tm_mon  = rtcDate.month - 1;
            localTime.tm_mday = rtcDate.date;
            localTime.tm_wday = rtcDate.weekDay;
            localTime.tm_hour = rtcTime.hours;
            localTime.tm_min  = rtcTime.minutes;
            localTime.tm_sec  = rtcTime.seconds;

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
        
        // Set ntp time to local
        configTzTime(MY_TIMEZONE, MY_NTP_SERVER);

        time_t now;
        do {
            time(&now);
            std::cerr << "." << std::flush;
            delay(500);
        }
        while(now < 1660275195L); // arbitrary time_t value > 0

        std::tm timeInfo; // the structure tm holds time information in a more convenient way
        localtime_r(&now, &timeInfo); // update the structure tm with the current time

        I2C_BM8563_TimeTypeDef timeStruct;
        timeStruct.hours    = timeInfo.tm_hour;
        timeStruct.minutes  = timeInfo.tm_min;
        timeStruct.seconds  = timeInfo.tm_sec;
        rtc.setTime(&timeStruct);

        // Set RTC Date
        I2C_BM8563_DateTypeDef dateStruct;
        dateStruct.weekDay  = timeInfo.tm_wday;
        dateStruct.month    = timeInfo.tm_mon + 1;
        dateStruct.date     = timeInfo.tm_mday;
        dateStruct.year     = timeInfo.tm_year + 1900;
        rtc.setDate(&dateStruct);

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

        I2C_BM8563_TimeTypeDef timeStruct;
        rtc.getTime(&timeStruct);

        I2C_BM8563_DateTypeDef dateStruct;
        rtc.getDate(&dateStruct);

        std::cerr << "System clock Date: " << localTime.tm_year + 1900 << "/" << localTime.tm_mon + 1 << "/" << localTime.tm_mday << std::endl;
        std::cerr << "System clock Time: " << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << std::endl;

        std::cerr << "RTC Date:          " << dateStruct.year << "/" << (int)dateStruct.month << "/" << (int)dateStruct.date << std::endl;
        std::cerr << "RTC clock Time:    " << (int)timeStruct.hours << ":" << (int)timeStruct.minutes << ":" << (int)timeStruct.seconds << std::endl;
    }

}
