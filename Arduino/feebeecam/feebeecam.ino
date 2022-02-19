#include <Arduino.h>
#include <bee-fish.h>
#include <FS.h>
#include <SPIFFS.h>
#include "wifi.h"
#include "web-server.h"
#include "file-system.h"
//#include "camera.h"
#include "memory.h"
#include "i2c.h"
#include "light.h"
#include "weather.h"
//#include "battery.h"

void setup() {
    
    using namespace FeebeeCam;
    
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    logMemory();

    initializeWiFi();

    initializeFileSystem();

    initializeCamera();

    initializeWebServers();

    initializeI2C();

  //  initializeBattery();

//    Light light;
//    light.turnOff();

    logMemory();
}



void loop() {
}