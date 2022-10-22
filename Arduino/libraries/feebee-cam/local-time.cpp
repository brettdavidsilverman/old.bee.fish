#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <iostream>
#include "local-time.h"
#include "setup.h"
#include "config.h"

namespace FeebeeCam {

    using namespace std;
    using namespace BeeFishBString;
    
    bool initializeTime() {

        cerr << "Time initializing" << endl;

        BString timeZone = MY_TIMEZONE;

        if ( FeebeeCam::_setup && 
             FeebeeCam::_setup->_timeZone.length() )
        {
            timeZone = FeebeeCam::_setup->_timeZone;
        }

        std::string _timeZone = timeZone.str();
        
        configTzTime(_timeZone.c_str(), MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line

        cerr << "Waiting for time from internet" << endl;

        unsigned long timeout = millis() + WEB_REQUEST_TIMEOUT;

        while (!isTimeInitialized() && timeout > millis()) {
            delay(500);
            Serial.print(".");
        }

        cerr << endl << FeebeeCam::getDateTime() << endl;

        return true;

    }

    bool isTimeInitialized()
    {

        time_t now;        // this is the epoch

        time(&now);                    // read the current time

        if (now < 1660275195L) {
            return false;
        }

        return true;

    }

    BString getDate() {
        
        std::time_t now;
        time(&now);
        
        std::tm* localDate = std::localtime(&now);

        std::stringstream stream;
        
        static const char* Months [] = {
            "Jan",
            "Feb",
            "Mar",
            "Apr",
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

    BString getTime(std::time_t* now) {
        
        std::time_t _now;
        if (now == nullptr) {
            now = &_now;
            time(now);
        }

        std::tm* localTime = std::localtime(now);

        std::stringstream stream;
        
        stream  << std::setfill('0') << std::setw(2) << localTime->tm_hour << ":"
                << std::setfill('0') << std::setw(2) << localTime->tm_min << ":"
                << std::setfill('0') << std::setw(2) << localTime->tm_sec;

        return stream.str();
    }

    BString getDateTime() {

        BString dateTime = 
            FeebeeCam::getDate() + " " + 
            FeebeeCam::getTime();

        return dateTime;
    }
}