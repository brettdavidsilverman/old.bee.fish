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
            1,     // Priority of the task. 
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
                default:
                    ;
                }
            }
            delay(190);
        }
    }


}