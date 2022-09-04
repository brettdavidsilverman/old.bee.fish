#include <iostream>
#include <time.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <feebee-cam.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void displayNow();

void setup() {
   
   SerialBT.begin("ESP32test"); //Bluetooth device name

   Serial.begin(1500000);
   while (!Serial)
      delay(10);

   Serial.print  ("Real Time Clock Example Starting\r\n");
   
   SerialBT.print("Real Time Clock Example Starting\r\n");
   
   if (!FeebeeCam::initializeBattery()) {
      SerialBT.print("Battery initialization failed\r\n");
      while (1)
         ;
   }

   FeebeeCam::initializeRTC();

   if (!FeebeeCam::initializeLight()) {
      SerialBT.print("Light initialization failed\r\n");
      while (1)
         ;
   }

   FeebeeCam::light->turnOn();

   if (!FeebeeCam::initializeFileSystem()) {
      SerialBT.print("File System initialization failed\r\n");
      while (1)
         ;
   }

   if (!FeebeeCam::initializeSetup()) {
      SerialBT.print("Setup initialization failed\r\n");
      while (1)
         ;
   }

   if (!FeebeeCam::initializeWiFi()) {
      SerialBT.print("WiFi initialization failed\r\n");
      while (1)
         ;
   }

   while (!WiFi.isConnected()) {
      delay(500);
      SerialBT.print(".");
   }


   SerialBT.print("RTC Initialized.\r\n");

   delay(1000);

   displayNow();

   Serial.print("Real Time Clock Ready\r\n");

   while (1) {
      
      String line = SerialBT.readString();
      
      line.trim();

      if (line.length() == 0)
         continue;

      if (line == "set") {
         FeebeeCam::initializeRTC();
         displayNow();
      }
      else if (line == "time") {
         displayNow();
      }
      else if (line == "sleep") {
         FeebeeCam::light->turnOff();
         FeebeeCam::rtc->clearIRQ();
         int result = FeebeeCam::rtc->SetAlarmIRQ(10);
         delay(10);

         bat_disable_output();

         SerialBT.print("Setting sleep instead\r\n");

         esp_deep_sleep(1000L * 1000L * 5L);

      }
      else {
         SerialBT.print("Commands [set] [time] [sleep]\r\n");
      }
   }
      

}

void loop() {
   

}

void displayNow() {

   time_t now = time(NULL);
   std::tm* localTime = localtime(&now);

   I2C_BM8563_TimeTypeDef timeStruct;
   FeebeeCam::rtc->getTime(&timeStruct);

   I2C_BM8563_DateTypeDef dateStruct;
   FeebeeCam::rtc->getDate(&dateStruct);

   std::stringstream stream;

   stream << "System clock Date: " << localTime->tm_year + 1900 << "/" << localTime->tm_mon + 1 << "/" << localTime->tm_mday << std::endl;
   stream << "System clock Time: " << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec << std::endl;

   stream << "RTC Date:          " << dateStruct.year << "/" << (int)dateStruct.month << "/" << (int)dateStruct.date << std::endl;
   stream << "RTC clock Time:    " << (int)timeStruct.hours << ":" << (int)timeStruct.minutes << ":" << (int)timeStruct.seconds << std::endl;

   SerialBT.print(stream.str().c_str());

}
