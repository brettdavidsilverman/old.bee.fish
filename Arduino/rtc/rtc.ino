#include <iostream>
#include <time.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <feebee-cam.h>

void showlog(rtc_time_type *rtcTime, rtc_date_type * rtcDate) {
      
      time_t now;

      time(&now);            // this is the epoch
      std::cerr << now << std::endl;
      std::tm localTime; // the structure tm holds time information in a more convenient way
      localtime_r(&now, &localTime); // update the structure tm with the current time
      
      std::cerr << "local date " << localTime.tm_year + 1900 << "/" << localTime.tm_mon + 1 << "/" << localTime.tm_mday << std::endl;
      std::cerr << "local time " << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << std::endl;

      std::cerr << "RTC date   " << (int)rtcDate->Year << "/" << (int)rtcDate->Month << "/" << (int)rtcDate->Date << std::endl;
      std::cerr << "RTC time   " << (int)rtcTime->Hours << ":" << (int)rtcTime->Minutes << ":" << (int)rtcTime->Seconds << std::endl;

}

void setup() {
   
   Serial.begin(1500000);
   while (!Serial)
      delay(10);

   std::cerr << "Real Time Clock Example Starting" << std::endl;
   
   if (!FeebeeCam::initializeSetup()) {
      std::cerr << "Setup initialization failed" << std::endl;
      while (1)
         ;
   }

   std::cerr << "Connecting to " << DEFAULT_SSID << std:: endl;
   
   WiFi.begin(DEFAULT_SSID, DEFAULT_PASSWORD);

   if (!FeebeeCam::isRTCInitialized()) {
      std::cerr << "RTC hasn't been set. We need the internet for time" << std::endl;
      while (!WiFi.isConnected()) {
         delay(500);
         Serial.print(".");
      }
      Serial.println(" connected");

   }
         
   if (!FeebeeCam::initializeRTC()) {
      std::cerr << "RTC initialization failed" << std::endl;
      while (1)
         ;
   }

   std::cerr << "RTC Initialized" << std::endl;

}

void loop()
{
   if (Serial.available()) {
      Serial.readString();
      std::cerr << "Setting interrupt for 10 seconds" << std::endl;
      FeebeeCam::RTC.setAlarmIRQ(10);
      std::cerr << "Powering down" << std::endl;
      
      FeebeeCam::initializeRTC(true);
   }

   rtc_time_type time;
   rtc_date_type date;

   FeebeeCam::RTC.getTime(&time);
   FeebeeCam::RTC.getDate(&date);

   showlog(&time, &date);

   delay(1000);
}
