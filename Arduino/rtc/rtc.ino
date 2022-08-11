#include <iostream>
#include <time.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <feebee-cam.h>

void setup() {
   
   Serial.begin(1500000);
   while (!Serial)
      delay(10);

   std::cerr << "Real Time Clock Example Starting" << std::endl;

   if (!FeebeeCam::initializeLight()) {
      std::cerr << "Light initialization failed" << std::endl;
      while (1)
         ;
   }

   FeebeeCam::light->turnOn();

   if (!FeebeeCam::initializeBattery()) {
      std::cerr << "Battery initialization failed" << std::endl;
      while (1)
         ;
   }

   if (!FeebeeCam::initializeSetup()) {
      std::cerr << "Setup initialization failed" << std::endl;
      while (1)
         ;
   }

   std::cerr << "Is rtc initialized: " << (FeebeeCam::isRTCInitialized() ? "true" : "false") << std::endl;

   if (FeebeeCam::isRTCInitialized() == false) {
      if (!FeebeeCam::initializeWiFi()) {
         std::cerr << "WiFi initialization failed" << std::endl;
         while (1)
            ;
      }

   }

   if (!FeebeeCam::initializeRTC()) {
      std::cerr << "RTC initialization failed" << std::endl;
      while (1)
         ;
   }

   std::cerr << "RTC Initialized" << std::endl;

}

void loop() {
   
   FeebeeCam::displayNow();
   
   rtc_time_type time;
   rtc_date_type date;

   FeebeeCam::RTC.getTime(&time);
   FeebeeCam::RTC.getDate(&date);

   std::cerr << "Setting interrupt for 10 seconds" << std::endl;
   
   FeebeeCam::RTC.setAlarmIRQ(10);

   std::cerr << "Powering down" << std::endl;

   FeebeeCam::light->turnOff();

   bat_disable_output();

   esp_deep_sleep(1000L * 1000L * 5L);

   esp_deep_sleep_start();

}
