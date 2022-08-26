#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <iostream>
#include "local-time.h"
#include "config.h"

namespace FeebeeCam {

    time_t lastTimeCameraUsed = 0;

    bool initializeTime() {

        std::cerr << "Setting up time" << std::endl;

        configTzTime(MY_TIMEZONE, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line

        std::cerr << "Time initializing" << std::endl;
      
        while (!isTimeInitialized()) {
            delay(500);
            Serial.print(".");
        }

        if (!FeebeeCam::setLastTimeCameraUsed())
            return false;
            
        std::cerr << "Time Initialized" << std::endl;

        return true;

    }

    bool isTimeInitialized()
    {

        time_t now;        // this is the epoch

        time(&now);                    // read the current time

        if (now < 1660275195L) {
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