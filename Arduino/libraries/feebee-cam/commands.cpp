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
//                    FeebeeCam::downloadRequiredFiles();
                    FeebeeCam::uploadSettings();
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
            delay(19);
        }
    }

    void putToSleep() {

        FeebeeCam::BeeFishStorage storage("/beehive/");

        const long checkEvery = (double)settings["checkEvery"] ;
        long sleepTimeMicroSeconds = checkEvery * 1000L * 1000L;

        settings["sleeping"] = true;
        settings["wakeup"] = false;

        cerr << "Saving settings to storage" << endl;
        storage.setItem("settings", settings);

        cerr << settings << endl;

        Serial.print("Putting to sleep for ");
        Serial.print(checkEvery);
        Serial.println(" seconde");

        esp_sleep_enable_timer_wakeup(sleepTimeMicroSeconds);
        
        esp_deep_sleep_start();

    }

    bool uploadSettings() {

        FeebeeCam::BeeFishStorage storage("/beehive/");
        BeeFishBScript::Variable variable = storage.getItem("settings");

        bool result = true;

        cerr << "Variable: " << variable << endl;

        if (variable == nullptr || variable == undefined) {
            cerr << "Creating default settings" << endl;
            FeebeeCam::settings.clear();
            FeebeeCam::settings["checkEvery"] = 30;
            FeebeeCam::settings["photographMinutes"] = 1;
            FeebeeCam::settings["wakeup"] = false;
        }
        else {
            cerr << "Using settings from cloud" << endl;
            FeebeeCam::settings = *(BeeFishBScript::ObjectPointer)variable;
            bool wakeup = false;

            if (FeebeeCam::settings.contains("wakeup"))
                wakeup = FeebeeCam::settings["wakeup"];
            
            if (!wakeup) {
                FeebeeCam::putToSleep();
                return true;
            }
        }

        FeebeeCam::settings["label"] = FeebeeCam::setup._label,
        FeebeeCam::settings["url"] = FeebeeCam::getURL();
        FeebeeCam::settings["sleeping"] = false;

        result = storage.setItem("settings", FeebeeCam::settings);
        
        if (result)
            clog << "Uploaded beehive settings" << endl;
        else
            clog << "Error uploading beehive settings" << endl;

        return result;

    }

}