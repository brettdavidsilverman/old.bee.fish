#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <iostream>
#include "local-time.h"
#include "config.h"

namespace FeebeeCam {

    extern time_t lastTimeCameraUsed = 0;

    bool initializeTime() {

        std::cerr << "Setting up time" << std::endl;

        configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
        setenv("TZ", MY_TIMEZONE, 1);          // Set environment variable with your time zone
        tzset();

        std::cerr << "Time initializing" << std::endl;
      
        while (!isTimeInitialized()) {
            delay(500);
            Serial.print(".");
        }

        std::cerr << "Initialized" << std::endl;

        return true;

    }

    bool isTimeInitialized()
    {

        time_t now;        // this is the epoch
        std::tm localTime; // the structure tm holds time information in a more convenient way

        time(&now);                    // read the current time
        localtime_r(&now, &localTime); // update the structure tm with the current time

        if (localTime.tm_year + 1900 < 2000) {
            return false;
        }
        else {
            return true;
        }

        return false;

    }

    bool setLastTimeCameraUsed() {
        std::time(&lastTimeCameraUsed);
        return true;
    }

}