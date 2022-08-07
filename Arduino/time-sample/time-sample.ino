#include <bee-fish.h>
#include <Arduino.h>
#include <WiFi.h>
#include "local-time.h"

#ifndef STASSID
#define STASSID "laptop"     // set your SSID
#define STAPSK "feebeegeeb3" // set your wifi password
#endif

void setup()
{
    Serial.begin(1500000);
    Serial.println("\nNTP TZ DST - bare minimum");


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
    
    setupTime();
}

void loop()
{
    if (isTimeSetup()) {
        std::cerr << BeeFishId::Timestamp::epoch() << std::endl;
    }
    else {
        std::cerr << "Time not ready" << std::endl;
    }

    delay(500);
}
