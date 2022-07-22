#include <iostream>

#include "feebee-cam.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "commands.h"
#include "weather.h"

namespace FeebeeCam {

    Commands commands;
    std::mutex guard;

    bool initializeCommands() {

        return true;
    }


    void handleCommands() {
        
        while (!commands.empty()) {
            command_t command = commands.pop();

            switch (command) {
            
            case INTERNET:
                
                FeebeeCam::initializeSettings();
                if ((bool)settings["wakeup"]) {
                    FeebeeCam::downloadRequiredFiles();
                    FeebeeCam::initializeWebServer();
                }
                else {
                    FeebeeCam::uploadWeatherReport();    
                    FeebeeCam::putToSleep();
                }
                break;

            case INITIALIZE_WEBSERVER:

                FeebeeCam::initializeWebServer();
                break;

            case SAVE_SETUP:
                FeebeeCam::setup.save();
                break;

            case UPLOAD_WEATHER:
                FeebeeCam::uploadWeatherReport();
                break;

            case PUT_TO_SLEEP:
                putToSleep();
                break;

            case RESTART:
                ESP.restart();
                break;

            default:
                ;
            }
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

        Serial.print("Putting to sleep for ");
        Serial.print(checkEvery);
        Serial.println(" seconde");

        esp_sleep_enable_timer_wakeup(sleepTimeMicroSeconds);
        
        esp_deep_sleep_start();

    }

    bool initializeSettings() {

        FeebeeCam::BeeFishStorage storage("/beehive/");
        BeeFishBScript::Variable variable = storage.getItem("settings");

        bool result = true;

        if (variable == nullptr || variable == undefined) {
            cerr << "Creating default settings" << endl;
            FeebeeCam::settings.clear();
            FeebeeCam::settings["checkEvery"] = 30;
            FeebeeCam::settings["photographMinutes"] = 1;
            FeebeeCam::settings["wakeup"] = true;
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
        
        cerr << FeebeeCam::settings << endl;

        if (result)
            clog << "Uploaded beehive settings" << endl;
        else
            clog << "Error uploading beehive settings" << endl;

        return result;

    }

}