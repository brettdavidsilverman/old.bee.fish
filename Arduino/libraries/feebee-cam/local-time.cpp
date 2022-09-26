#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <iostream>
#include "local-time.h"
#include "setup.h"
#include "light.h"
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

        configTzTime(timeZone.c_str(), MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line

        cerr << "Waiting for time from internet" << endl;

        unsigned long timeout = millis() + WEB_REQUEST_TIMEOUT;

        while (!isTimeInitialized() && timeout > millis()) {
            Serial.print(".");
            FeebeeCam::light->flash(100, 1);
            delay(500);
        }

        if (!isTimeInitialized())
            FeebeeCam::restartAfterError();

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

    BString getDate(std::time_t* now) {
        
        std::time_t _now;
        if (now == nullptr) {
            now = &_now;
            time(now);
        }
        
        std::tm* localDate = std::localtime(now);

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

    double getEpoch() {

        std::chrono::system_clock::time_point timeNow 
            = std::chrono::system_clock::now();

        unsigned long epoch =
            std::chrono::duration_cast<std::chrono::seconds>(
            timeNow.time_since_epoch()).count();

        return epoch;
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

    BString getDateTime(std::time_t* now) {

        std::time_t _now;
        if (now == nullptr) {
            now = &_now;
            time(now);
        }

       BString dateTime = 
            FeebeeCam::getDate(now) + " " + 
            FeebeeCam::getTime(now);

        return dateTime;
    }
}