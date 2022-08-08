#include <iostream>
#include <time.h>
#include <WiFi.h>
#include "local-time.h"
#include "rtc.h"


char str_buffer[64];

void showlog(rtc_time_type *rtc_time, rtc_date_type *rtc_date) {
      sprintf(str_buffer,"RTC Date Now is %02d:%02d:%02d WeekDay:%02d",
               rtc_date->Year, rtc_date->Month, rtc_date->Date, rtc_date->WeekDay);
      Serial.println(str_buffer);
      sprintf(str_buffer,"RTC Time Now is %02d:%02d:%02d",
               rtc_time->Hours, rtc_time->Minutes, rtc_time->Seconds);
      Serial.println(str_buffer);
}

void setup() {
   
   Serial.begin(1500000);
   while (!Serial)
      delay(10);

   WiFi.begin();                         // Connect to the access point of the last connection
   //WiFi.begin("SSID", "KEY");   // Or, Connect to the specified access point

   Serial.print("Connecting to Wi-Fi ");
   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   }
   Serial.println(" connected");

   if (!FeebeeCam::isRTCInitialized()) {
      FeebeeCam::initializeTime();
      FeebeeCam::initializeRTC();
   }
         

}

void loop()
{
   rtc_time_type time;
   rtc_date_type date;

   FeebeeCam::RTC.getTime(&time);
   FeebeeCam::RTC.getDate(&date);

   showlog(&time, &date);

   delay(1000);
}
