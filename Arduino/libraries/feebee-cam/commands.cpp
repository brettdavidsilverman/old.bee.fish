#include <iostream>

#include <feebee-cam.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_wifi.h>
#include "commands.h"
#include "weather.h"
#include "setup.h"
#include "config.h"

namespace FeebeeCam {

    Commands commands;
    std::mutex guard;

    bool initializeCommands() {

        return true;
    }

    void onConnectedToInternet();


    void handleCommands() {

        std::lock_guard<std::mutex> lock(guard);
        
        while (!commands.empty()) {

            command_t command = commands.pop();

            switch (command) {
            
                case INTERNET:

                    FeebeeCam::onConnectedToInternet() ;
                    break;

                case INITIALIZE_WEBSERVER:

                    throw 1;
                    //FeebeeCam::initializeWebServer();
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

                case DOWNLOAD_FILES:
                    FeebeeCam::downloadFiles();
                    break;

                case RESTART:
                    ESP.restart();
                    break;

                default:
                    ;
            }
        }
    }

    bool putToSleep() {


        // Stop the camera if running
        if (FeebeeCam::isCameraRunning) {
            
            FeebeeCam::stop = true;
        
            std::cerr << "Waiting for camera to stop" << std::endl;

            // Wait for camera to stop
            while (FeebeeCam::isCameraRunning)
                delay(10);
            
        }

        FeebeeCam::BeeFishStorage storage("/beehive/");
        
        if (!settings.contains("checkEvery"))
            settings["checkEvery"] = CHECK_EVERY_SECONDS;

        const long checkEvery = (double)settings["checkEvery"] ;
        long sleepTimeMicroSeconds = checkEvery * 1000L * 1000L;

        settings["sleeping"] = true;
        settings["wakeup"] = false;

        if (!storage.setItem("settings", settings)) {
            std::cerr << "Couldnt set sleep/wakeup settings" << std::endl;
            //FeebeeCam::resetConnection();
            //return false;
        }

        Serial.print("Putting to sleep for ");
        Serial.print(checkEvery);
        Serial.println(" seconde");

        FeebeeCam::light->turnOff();

        Serial.flush();

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
        }

        FeebeeCam::settings["label"] = FeebeeCam::setup._label,
        FeebeeCam::settings["url"] = FeebeeCam::getURL();
        FeebeeCam::settings["sleeping"] = false;

        result = storage.setItem("settings", FeebeeCam::settings);
        
        if (result)
            clog << "Uploaded beehive settings" << endl;
        else {
            clog << "Error uploading beehive settings" << endl;
            FeebeeCam::resetConnection();
            return false;
        }

        return result;

    }


}