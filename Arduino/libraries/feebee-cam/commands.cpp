#include <iostream>

#include "feebee-cam.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "commands.h"

namespace FeebeeCam {

    Commands commands;

    void commandLoop(void*);

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

    void putToSleep(long sleepTimeMicroSeconds) {

        if (sleepTimeMicroSeconds <= 0) {

            FeebeeCam::BeeFishStorage storage("/beehive/");

            BeeFishBScript::ObjectPointer status = storage.getItem("status");

            const long checkEvery = (double)(*status)["checkEvery"] ;
            sleepTimeMicroSeconds = checkEvery * 1000L * 1000L;
        }

        Serial.print("Putting to sleep for ");
        Serial.print(sleepTimeMicroSeconds);
        Serial.println(" micro seconde");

        esp_sleep_enable_timer_wakeup(sleepTimeMicroSeconds);
        
        esp_deep_sleep_start();

    }    


}