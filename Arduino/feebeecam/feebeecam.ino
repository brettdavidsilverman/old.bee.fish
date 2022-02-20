#include <Arduino.h>
#include <stdexcept>
#include <bee-fish.h>
#include <FS.h>
#include <SPIFFS.h>
#include "wifi.h"
#include "web-server.h"
#include "file-system.h"
#include "camera.h"
#include "memory.h"
#include "i2c.h"
#include "light.h"
#include "weather.h"
//#include "battery.h"

void stackedSetup(void*);
void stackedLoop();


void setup() {

    // Redefine setup and loop with greater stack size

    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    
    xReturned = xTaskCreatePinnedToCore(
        stackedSetup,         /* Task function. */
        "stackedSetup",      /* String with name of task. */
        20000,               /* Stack size in bytes. */
        nullptr,                /* Parameter passed as input of the task */
        1,                     /* Priority of the task. */
        &xHandle            /* Task handle. */,
        1                      /* Pinned to core */
    );         

    vTaskDelete(NULL);

}

void loop() {
    throw std::logic_error("This default task should have been deleted");    
}

void stackedSetup(void*) {

    using namespace FeebeeCam;
    
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    logMemory();

    initializeFileSystem();
    
    initializeWiFi();

    initializeCamera();

    initializeWebServers();

    initializeI2C();

    initializeWeather();

    Light light(&wire);
    light.turnOff();

    logMemory();

    for (;;) {
        stackedLoop();
        delay(10);
    }
}

void stackedLoop() {
 
    using namespace FeebeeCam;

    if (!WiFi.isConnected()) {
        WiFi.begin();
        delay(5000);
    }
    else {
        if (FeebeeCam::downloadWhenReady) {
            FeebeeCam::downloadWhenReady = false;
            downloadRequiredFiles();
        }        
    }

 }

