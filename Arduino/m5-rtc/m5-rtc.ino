#include "M5_BM8563.h"
#include <WiFi.h>


const char* ntpServer = "pool.ntp.org";

// I2C pin definition for M5Stack TimerCam
// RTC BM8563 I2C port
#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14
// Grove I2C port
#define GROVE_I2C_SDA 4
#define GROVE_I2C_SCL 13

BM8563 RTC;
TwoWire wire(3);

rtc_time_type rtc_time_unit;
rtc_date_type rtc_date_unit;

char str_buffer[64];

void showlog(rtc_time_type *rtc_time, rtc_date_type *rtc_date) {
    sprintf(str_buffer,"RTC Time Now is %02d:%02d:%02d",
          rtc_time->Hours, rtc_time->Minutes, rtc_time->Seconds);
    Serial.println(str_buffer);
    sprintf(str_buffer,"RTC Date Now is %02d:%02d:%02d WeekDay:%02d",
          rtc_date->Year, rtc_date->Month, rtc_date->Date, rtc_date->WeekDay);
    Serial.println(str_buffer);
}

void setup()
{
  Serial.begin(1500000);
  while (!Serial)
    delay(10);

  Serial.println("Starting up");

  wire.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);
  RTC.begin(&wire);

  Serial.println("Ok");
  

  // Connect to an access point
  //WiFi.begin();                 // Connect to the access point of the last connection
  WiFi.begin("Laptop", "feebeegeeb3");  // Or, Connect to the specified access point

  Serial.print("Connecting to Wi-Fi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  // Set ntp time to local
  configTime(9 * 3600, 0, ntpServer);
  
// Get local time
  struct tm timeInfo;
  if (getLocalTime(&timeInfo)) {
    rtc_time_unit.Hours = timeInfo.tm_hour;
    rtc_time_unit.Minutes = timeInfo.tm_min;
    rtc_time_unit.Seconds = timeInfo.tm_sec;
  
    rtc_date_unit.WeekDay = timeInfo.tm_wday + 1;
    rtc_date_unit.Month = timeInfo.tm_mon + 1;
    rtc_date_unit.Date = timeInfo.tm_mday;
    rtc_date_unit.Year = timeInfo.tm_year + 1900;

    RTC.setTime(&rtc_time_unit);
    RTC.setDate(&rtc_date_unit);
  }
}

void loop()
{

  RTC.getTime(&rtc_time_unit);
  RTC.getDate(&rtc_date_unit);

  showlog(&rtc_time_unit, &rtc_date_unit);
  delay(1000);
}
