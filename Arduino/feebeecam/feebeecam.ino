#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <bee-fish.h>
#include "wifi.h"
#include "web-server.h"
#include "load-spiffs.h"
#include "camera.h"
#include "memory.h"

void setup() {
    
    using namespace FeebeeCam;
    
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    logMemory();

    initializeWiFi();

    Serial.println("Press any key to load bee hive files...");
    delay(2000);

    if (Serial.available()) {
        Serial.println("Loading bee hive files");
        loadBeeHive();
    }

    if(!SPIFFS.begin(true)){
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    initializeCamera();

    initializeWebServer();

    logMemory();
}



void loop() {

}