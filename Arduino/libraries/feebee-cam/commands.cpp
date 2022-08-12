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

                    //FeebeeCam::onConnectedToInternet() ;
                    break;

                case INITIALIZE_WEBSERVER:

                    //FeebeeCam::initializeWebServer();
                    break;

                case SAVE_SETUP:
                    FeebeeCam::_setup->save();
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
                    std::cerr << "Restarting now" << std::endl;
                    FeebeeCam::restart();
                    break;

                default:
                    ;
            }
        }
    }

    bool onCommand(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishBScript::Object object;
        object["status"] = BeeFishBScript::Null();
        object["message"] = "Invalid command";

        BeeFishBScript::ObjectPointer request = 
            (BeeFishBScript::ObjectPointer)(client->body());

        // Command
        BString command = (*request)["command"];

        bool _putToSleep = false;
        bool _downloadFiles = false;
        bool restart = false;

        if (command == "stop") {
            FeebeeCam::stop = true;
            object["status"] = true;
            object["message"] = "Camera stopped";
        }
        else if (command == "reset") {
            _setup->reset();
            _setup->save();
            _setup->applyToCamera();
            object["status"] = true;
            object["message"] = "Camera reset";
        }
        else if (command == "sleep") {
            object["status"] = true;
            object["message"] = "Camera put to sleep";
            object["redirectURL"] = HOST "/beehive/";
            _putToSleep = true;
        }
        else if (command == "restart") {
            object["status"] = true;
            object["message"] = "Camera restarting";
            object["redirectURL"] = HOST "/beehive/";
            restart = true;
        }
        else if (command == "download") {
            object["status"] = true;
            object["message"] = "Downloading new firmware";
            object["statusURL"] = HOST "download";
            _downloadFiles = true;
        }
                
        
        Serial.print("Sent Camera command ");
        Serial.println(command.c_str());

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        client->_contentType = "text/javascript";
        client->sendHeaders();

        stream << object;

        client->sendFinalChunk();

        if (_putToSleep) {
            FeebeeCam::commands.push(FeebeeCam::PUT_TO_SLEEP);
        }

        if (_downloadFiles) {
            FeebeeCam::commands.push(FeebeeCam::DOWNLOAD_FILES);
        }

        if (restart) {
            FeebeeCam::commands.push(FeebeeCam::RESTART);
        }
        
        return true;

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
            //FeebeeCam::restart();
            //return false;
        }

        Serial.print("Putting to sleep for ");
        Serial.print(checkEvery);
        Serial.println(" seconds");

        Serial.flush();

        FeebeeCam::light->turnOff();

        //FeebeeCam::initializeRTC();
        FeebeeCam::RTC.setAlarmIRQ(checkEvery);

        bat_disable_output();

        esp_deep_sleep(1000L * 1000L * (unsigned long)checkEvery);

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

        FeebeeCam::settings["label"] = FeebeeCam::_setup->_label,
        FeebeeCam::settings["url"] = FeebeeCam::getURL();
        FeebeeCam::settings["sleeping"] = false;

        result = storage.setItem("settings", FeebeeCam::settings);
        
        if (result)
            clog << "Uploaded beehive settings" << endl;
        else {
            clog << "Error uploading beehive settings" << endl;
            FeebeeCam::restart();
            return false;
        }

        return result;

    }

    void restart() {
        nvs_flash_deinit();
        ESP.restart();
    }

}