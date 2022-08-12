#include <WiFi.h>
#include <iostream>
#include "I2C_BM8563.h"

// RTC BM8563 I2C port
// I2C pin definition for M5Stick & M5Stick Plus & M5Stack Core2
//#define BM8563_I2C_SDA 21
//#define BM8563_I2C_SCL 22

// I2C pin definition for M5Stack TimerCam
#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14

#define MY_TIMEZONE "AEST-10"

I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire1);

const char* ntpServer = "time.cloudflare.com";

void setup() {
   // Init Serial
   Serial.begin(1500000);
   delay(50);

   // Connect to an access point
   WiFi.begin("Android", "feebeegeeb3");   // Or, Connect to the specified access point

   Serial.print("Connecting to Wi-Fi ");
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }
   Serial.println(" CONNECTED");

   // Init I2C
   Wire1.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);

   // Init RTC
   rtc.begin();

}

void loop() {


   if (Serial.readString() == "set") {

      // Set ntp time to local
      configTzTime(MY_TIMEZONE, ntpServer);

      time_t now;
      do {
         time(&now);
         std::cerr << "." << std::flush;
         delay(500);
      }
      while(now < 1660275195);
      
      // Get local time
      struct tm timeInfo;
      if (getLocalTime(&timeInfo)) {
         // Set RTC time
         I2C_BM8563_TimeTypeDef timeStruct;
         timeStruct.hours    = timeInfo.tm_hour;
         timeStruct.minutes = timeInfo.tm_min;
         timeStruct.seconds = timeInfo.tm_sec;
         rtc.setTime(&timeStruct);

         // Set RTC Date
         I2C_BM8563_DateTypeDef dateStruct;
         dateStruct.weekDay = timeInfo.tm_wday;
         dateStruct.month    = timeInfo.tm_mon + 1;
         dateStruct.date      = timeInfo.tm_mday;
         dateStruct.year      = timeInfo.tm_year + 1900;
         rtc.setDate(&dateStruct);
      }

   }
   else {

      // Get RTC
      I2C_BM8563_DateTypeDef dateStruct;
      I2C_BM8563_TimeTypeDef timeStruct;

      rtc.getDate(&dateStruct);
      rtc.getTime(&timeStruct);

      // Print RTC
      Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                           dateStruct.year,
                           dateStruct.month,
                           dateStruct.date,
                           timeStruct.hours,
                           timeStruct.minutes,
                           timeStruct.seconds
                        );


   }
   // Wait
   delay(1000);
}
