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

   if (!FeebeeCam::initializeFileSystem()) {
      std::cerr << "File System initialization failed" << std::endl;
      while (1)
         ;
   }

   if (!FeebeeCam::initializeSetup()) {
      std::cerr << "Setup initialization failed" << std::endl;
      while (1)
         ;
   }

   if (!FeebeeCam::initializeBattery()) {
      std::cerr << "Battery initialization failed" << std::endl;
      while (1)
         ;
   }

   std::cerr << "Is rtc Setup: " << (FeebeeCam::isRTCSetup() ? "true" : "false") << std::endl;

   if (!FeebeeCam::isRTCSetup()) {
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

   std::cerr << "RTC Initialized." << std::endl;
   std:cerr << "Enter \"set\" to set the time" << std::endl;

   delay(1000);

   if (Serial.available()) {
      if (Serial.readString() == "set") {
         FeebeeCam::initializeRTC(true);
      }
   }

   FeebeeCam::displayNow();

   I2C_BM8563_TimeTypeDef time;
   I2C_BM8563_DateTypeDef date;

   FeebeeCam::rtc.getTime(&time);
   FeebeeCam::rtc.getDate(&date);

   std::cerr << "Setting interrupt for 10 seconds" << std::endl;
      
   FeebeeCam::rtc.SetAlarmIRQ(10);

   std::cerr << "Powering down" << std::endl;

   FeebeeCam::light->turnOff();

   bat_disable_output();

   esp_deep_sleep(1000L * 1000L * 5L);

}

void loop() {
   

}
