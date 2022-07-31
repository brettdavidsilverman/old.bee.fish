/*
NTP TZ DST - bare minimum
NetWork Time Protocol - Time Zone - Daylight Saving Time

Our target for this MINI sketch is:
- get the SNTP request running
- set the timezone
- (implicit) respect daylight saving time
- how to "read" time to be printed to Serial.Monitor

This example is a stripped down version of the NTP-TZ-DST (v2) from the ESP8266
and contains some #precompiler defines to make it working for
- ESP32 core 1.0.5, 1.0.6, 2.0.2
- ESP8266 core 2.7.4

by noiasca
2021-03-28
*/

//#include <credentials.h> // that's just for my unknown credentials ;-)

#include <bee-fish.h>
#include <Arduino.h>

#ifndef STASSID
#define STASSID "laptop"     // set your SSID
#define STAPSK "feebeegeeb3" // set your wifi password
#endif

/* Configuration of NTP */
// choose the best fitting NTP server pool for your country
#define MY_NTP_SERVER "au.pool.ntp.org"

// choose your time zone from this list
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define MY_TZ "AEST-10"

/* Necessary Includes */
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h> // we need wifi to get internet access
#endif
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#endif
#include <time.h> // time() ctime()

/* Globals */
time_t now;        // this is the epoch
std::tm localTime; // the structure tm holds time information in a more convenient way

bool showTime()
{

    time(&now);                    // read the current time
    localtime_r(&now, &localTime); // update the structure tm with the current time

    if (localTime.tm_year + 1900 < 2000) {
        std::cerr << "Not ready" << std::endl;
        return false;
    }
    else {
        std::cerr << "Milliseconds:" << BeeFishId::Timestamp::milliseconds() << std::endl;

        const long sleepTimeMicroSeconds = 10 * 1000L * 1000L;

        std::cerr << "Sleeping for 10 seconds...." << std::endl;

        esp_sleep_enable_timer_wakeup(sleepTimeMicroSeconds);

        esp_deep_sleep_start();

        return true;
    }

    return false;

/* 
    std::cerr << BeeFishId::Timestamp::milliseconds() << std::endl;

    Serial.print("year:");
    Serial.print(localTime.tm_year + 1900); // years since 1900
    Serial.print("\tmonth:");
    Serial.print(localTime.tm_mon + 1); // January = 0 (!)
    Serial.print("\tday:");
    Serial.print(localTime.tm_mday); // day of month
    Serial.print("\thour:");
    Serial.print(localTime.tm_hour); // hours since midnight 0-23
    Serial.print("\tmin:");
    Serial.print(localTime.tm_min); // minutes after the hour 0-59
    Serial.print("\tsec:");
    Serial.print(localTime.tm_sec); // seconds after the minute 0-61*
    Serial.print("\twday");
    Serial.print(localTime.tm_wday); // days since Sunday 0-6
    if (localTime.tm_isdst == 1)     // Daylight Saving Time flag
        Serial.print("\tDST");
    else
        Serial.print("\tstandard");
    Serial.println();
*/
}

void setup()
{
    Serial.begin(115200);
    Serial.println("\nNTP TZ DST - bare minimum");

#ifdef ARDUINO_ARCH_ESP32
    // ESP32 seems to be a little more complex:
    configTime(0, 0, MY_NTP_SERVER); // 0, 0 because we will use TZ in the next line
    setenv("TZ", MY_TZ, 1);          // Set environment variable with your time zone
    tzset();
#else
    // ESP8266
    configTime(MY_TZ, MY_NTP_SERVER); // --> for the ESP8266 only
#endif

    // start network
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(STASSID, STAPSK);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(200);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
}

void loop()
{
    showTime();
    delay(1000); // dirty delay
}