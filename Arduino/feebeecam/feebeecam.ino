#include <Arduino.h>
#include <stdexcept>
#include <bee-fish.h>
#include <FS.h>
#include <SPIFFS.h>
#include "wifi.h"
#include "web-server.h"
#include "file-system.h"
#include <camera.h>
#include "memory.h"
#include <light.h>
#include <weather.h>


/*
void stackedSetup(void*);
void stackedLoop();


void setupStacked() {

    // Redefine setup and loop with greater stack size

    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    
    xReturned = xTaskCreatePinnedToCore(
        stackedSetup,         // Task function.
        "stackedSetup",      // String with name of task.
        20000,               // Stack size in bytes. 
        nullptr,                // Parameter passed as input of the task 
        1,                     // Priority of the task.
        &xHandle            // Task handle.
        1                      // Pinned to core 
    );         

    vTaskDelete(NULL);

}

void loop() {
    throw std::logic_error("This default task should have been deleted");    
}
*/

void setup() {

    using namespace FeebeeCam;
    
    Serial.begin(1500000);
    while (!Serial)
        delay(10);

    logMemory();

    initializeFileSystem();
    
    initializeWiFi();

    initializeCamera();

    bat_init();

    initializeWebServers();

    Light light;
    light.rainbow();

    logMemory();
/*
    for (;;) {
        stackedLoop();
        delay(10);
    }
*/
}

void loop() {
 
    if (FeebeeCam::downloadWhenReady) {
        FeebeeCam::downloadWhenReady = false;
        FeebeeCam::downloadRequiredFiles();
    }        

}

