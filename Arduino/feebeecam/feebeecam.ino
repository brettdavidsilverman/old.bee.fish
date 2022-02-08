#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <bee-fish.h>
#include "wifi.h"
#include "web-server.h"

void setup() {
    
    using namespace FeebeeCam;
    
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    initializeWiFi();

    if(!SPIFFS.begin(true)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    initializeWebServer();

}


void loop() {

}