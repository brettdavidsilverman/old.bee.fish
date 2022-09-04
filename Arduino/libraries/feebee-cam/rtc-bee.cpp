#include <iostream>
#include <sys/time.h>
#include <sstream>
#include "rtc-bee.h"
#include "local-time.h"
#include "setup.h"
#include "config.h"
#include "local-time.h"

#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14

namespace FeebeeCam {

    I2C_BM8563* rtc = nullptr;

    bool setRTCDateTimeFromInternet();

    void onDateTimeFromInternet(void *);

    bool initializeRTC() {

        std::cerr  << "Initializing RTC" << std::endl;

        static bool initialized = false;
        static TwoWire myWire(1);

        if (initialized) {
            myWire.end();
            delete rtc;
            rtc = nullptr;
            initialized = false;
        }

        myWire.setPins(BM8563_I2C_SDA, BM8563_I2C_SCL);
        myWire.begin();
        rtc = new I2C_BM8563(I2C_BM8563_DEFAULT_ADDRESS, myWire);
        rtc->begin();
        rtc->clearIRQ();

        initialized = true;

        std::tm localTime{};

        I2C_BM8563_TimeTypeDef rtcTime;
        I2C_BM8563_DateTypeDef rtcDate;
        
        rtc->getTime(&rtcTime);
        delay(10);
        rtc->getDate(&rtcDate);
        delay(10);

        localTime.tm_year = rtcDate.year - 1900;
        localTime.tm_mon  = rtcDate.month - 1;
        localTime.tm_mday = rtcDate.date;
        localTime.tm_wday = rtcDate.weekDay;
        localTime.tm_hour = rtcTime.hours;
        localTime.tm_min  = rtcTime.minutes;
        localTime.tm_sec  = rtcTime.seconds;

        // Convert the local time to epoch
        time_t now = mktime(&localTime);

        struct timeval timeValue {};
        timeValue.tv_sec = now;
        cerr << "Set time of day from rtc" << endl;
        configTzTime(MY_TIMEZONE, MY_NTP_SERVER);
        int ret = settimeofday(&timeValue, NULL);
        if (ret == 0) {
            // Success
            cout << getDateTime() << std::endl;
        }
        else {
            std::cerr << "settimeofday failed with return value of " << ret << std::endl;
        }

        if (rtcDate.year >= 2020) {
            return true;
        }
        
        return setRTCDateTimeFromInternet();


    }

    void onDateTimeFromInternet(void *) {
        configTzTime(MY_TIMEZONE, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
        while (!isTimeInitialized()) {
            delay(500);
            Serial.print(".");
        }

        time_t now;
        time(&now);
        std::tm* timeInfo; // the structure tm holds time information in a more convenient way
        timeInfo = localtime(&now); // update the structure tm with the current time

        // Set RTC Date
        I2C_BM8563_DateTypeDef dateStruct;
        dateStruct.weekDay  = timeInfo->tm_wday;
        dateStruct.month    = timeInfo->tm_mon + 1;
        dateStruct.date     = timeInfo->tm_mday;
        dateStruct.year     = timeInfo->tm_year + 1900;
        
        rtc->setDate(&dateStruct);
        delay(10);

        I2C_BM8563_TimeTypeDef timeStruct;
        timeStruct.hours    = timeInfo->tm_hour;
        timeStruct.minutes  = timeInfo->tm_min;
        timeStruct.seconds  = timeInfo->tm_sec;
        rtc->setTime(&timeStruct);
        delay(10);

        cout << "Set date from internet" << endl;
        cout << getDateTime() << std::endl;

        vTaskDelete(NULL);

    }

    bool setRTCDateTimeFromInternet() {
        TaskHandle_t xHandle = NULL;
        xTaskCreatePinnedToCore(
            onDateTimeFromInternet,     // Task function. 
            "ntp",                      // String with name of task. 
            4028,                       // Stack size in bytes. 
            nullptr,                    // Parameter passed as input of the task 
            1,                          // Priority of the task. 
            &xHandle,                   // Task handle
            1                           // Pinned to core 
        );
        return xHandle != NULL;
    }
/*
    bool isRTCSetup() {

        I2C_BM8563_DateTypeDef dateStruct{};
        rtc->getDate(&dateStruct);

        return (dateStruct.year > 2021); // arbitrary year > 0

        //return FeebeeCam::_setup->_isRTCSetup;
    }
*/
    void displayNow() {

        time_t now = time(NULL);
        std::tm* localTime = localtime(&now);

        I2C_BM8563_TimeTypeDef timeStruct;
        rtc->getTime(&timeStruct);

        I2C_BM8563_DateTypeDef dateStruct;
        rtc->getDate(&dateStruct);

        std::cerr << "System clock Date: " << localTime->tm_year + 1900 << "/" << localTime->tm_mon + 1 << "/" << localTime->tm_mday << std::endl;
        std::cerr << "System clock Time: " << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec << std::endl;

        std::cerr << "RTC Date:          " << dateStruct.year << "/" << (int)dateStruct.month << "/" << (int)dateStruct.date << std::endl;
        std::cerr << "RTC clock Time:    " << (int)timeStruct.hours << ":" << (int)timeStruct.minutes << ":" << (int)timeStruct.seconds << std::endl;
    }

    BeeFishBString::BString getTime() {
        
        std::time_t now;
        time(&now);

        std::tm* localTime = std::localtime(&now);

        std::stringstream stream;
        
        stream  << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
                << std::setfill('0') << std::setw(2) << localTime->tm_min << ":"
                << std::setfill('0') << std::setw(2) << localTime->tm_sec;

        return stream.str();
    }

    BeeFishBString::BString getDate() {
        
        std::time_t now;
        time(&now);
        
        std::tm* localDate = std::localtime(&now);

        std::stringstream stream;
        
        static const char* Months [] = {
            "Jan",
            "Feb",
            "Mar",
            "May",
            "Jun",
            "Jul",
            "Aug",
            "Sep",
            "Oct",
            "Nov",
            "Dec"
        };

        stream  << std::setfill('0') << std::setw(2) << localDate->tm_mday << " "
                << Months[localDate->tm_mon] << " "
                << std::setfill('0') << std::setw(4) << (localDate->tm_year + 1900);
                

        return stream.str();
    }

    BeeFishBString::BString getDateTime() {

        BeeFishBString::BString dateTime = 
            FeebeeCam::getDate() + " " + 
            FeebeeCam::getTime();

        return dateTime;
    }

}
