#include <iostream>

#include "feebee-cam.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "commands.h"

namespace FeebeeCam {

    Commands commands;

    void commandLoop(void*);
    void putToSleep();

    bool initializeCommands() {
        TaskHandle_t xHandle = NULL;
                
        xTaskCreatePinnedToCore(
            commandLoop,      // Task function. 
            "commandLoop",      // String with name of task. 
            5000,                // Stack size in bytes. 
            NULL,                 // Parameter passed as input of the task 
            0,     // Priority of the task. 
            &xHandle,             // Task handle
            1                  // Pinned to core 
        );

        if (xHandle == NULL) {
            std::cerr << "Error starting command loop task" << std::endl;
            return false;
        }

        return (xHandle != NULL);
    }


    void commandLoop(void *) {
        for (;;) {
            while (!commands.empty()) {
                command_t command = commands.pop();

                switch (command) {
                case INITIALIZE:
                    FeebeeCam::downloadRequiredFiles();
                    FeebeeCam::setup.applyToCamera();
                    break;
                case SAVE_SETTINGS:
                    FeebeeCam::setup.save();
                    break;
                case UPLOAD_WEATHER:
                    FeebeeCam::uploadWeatherReport();
                    break;
                case PUT_TO_SLEEP:
                    putToSleep();
                    break;
                default:
                    ;
                }
            }
            delay(190);
        }
    }

    void putToSleep() {

        const long sleepTime = 10L * 1000L * 1000L;

        Serial.print("Putting to sleep for ");
        Serial.print(sleepTime / (1000L * 1000L));
        Serial.println(" seconde");

        setup._wakeup = false;
        setup._awake = false;

        setup.save();

        esp_sleep_enable_timer_wakeup(sleepTime);
        
        esp_deep_sleep_start();

    }    


}