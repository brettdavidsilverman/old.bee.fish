#include <iostream>

#include <feebee-cam.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include <esp_wifi.h>
#include "commands.h"
#include "weather.h"
#include "setup.h"
#include "config.h"
#include "rtc-bee.h"

namespace FeebeeCam {

    Commands commands;
    std::mutex guard;

    bool initializeCommands() {

        return true;
    }

    void handleCommands() {

        std::lock_guard<std::mutex> lock(guard);
        
        while (!commands.empty()) {

            command_t command = commands.pop();

            switch (command) {
            
                case INTERNET:

                    FeebeeCam::onConnectedToInternet();
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
                    FeebeeCam::downloadFiles(true, true);
                    break;

                case RESTART:
                    std::cerr << "Restarting now" << std::endl;
                    ESP.restart();;
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

        client->_contentType = "application/json";
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
            
            FeebeeCam::stopCamera();
        
        }

        if (!settings.contains("checkEvery"))
            settings["checkEvery"] = CHECK_EVERY_SECONDS;

        if (!settings.contains("takePictureEvery"))
            settings["takePictureEvery"] = TAKE_PICTURE_EVERY;

        const unsigned long checkEvery = (double)settings["checkEvery"] ;
        unsigned long sleepTimeMicroSeconds = checkEvery * 1000L * 1000L;

        settings["sleeping"] = true;
        settings["wakeup"] = false;

        std::chrono::system_clock::time_point timeNow 
            = std::chrono::system_clock::now();


        // Add checkEvery seconds to now, factoring in
        // time to save these settings
        timeNow += std::chrono::seconds(checkEvery + 2);
        
        time_t nextWakeup = std::chrono::system_clock::to_time_t(timeNow);

        settings["nextWakeupTime"]  = FeebeeCam::getTime(&nextWakeup);

        settings.save();        

        Serial.print("Putting to sleep for ");
        Serial.print(checkEvery);
        Serial.println(" seconds");

        Serial.flush();

        //FeebeeCam::rtc->clearIRQ();
        //int result = FeebeeCam::rtc->SetAlarmIRQ(5);
        //cerr << "SET ALARM IRQ RESULT " << result << endl;
        //delay(100);

        
        std::cerr << "Putting to deep sleep" << std::endl;

        FeebeeCam::light->flash(100, 2);
        FeebeeCam::light->turnOff();
        
        FeebeeCam::initializeRTC();

        bmm8563_clearIRQ();
        bmm8563_setTimerIRQ(checkEvery);
        //bmm8563_setDateIRQ(1, -1, -1, -1);


        esp_sleep_enable_timer_wakeup(sleepTimeMicroSeconds);

        // rtc wake up in 5 seconds

        bat_disable_output();

        esp_deep_sleep_start();


    }

    bool initializeSettings() {

        FeebeeCam::BeeFishStorage storage("/beehive/");
        BeeFishBScript::Variable variable = storage.getItem("settings");

        bool result = true;

        if (variable == nullptr || variable == undefined) {
            cerr << "Creating default settings" << endl;
            FeebeeCam::settings.clear();
            FeebeeCam::settings["checkEvery"] = CHECK_EVERY_SECONDS;
            FeebeeCam::settings["takePictureEvery"] = TAKE_PICTURE_EVERY;
            FeebeeCam::settings["wakeup"] = true;
        }
        else {
            cerr << "Using settings from cloud" << endl;

            FeebeeCam::settings.apply((BeeFishBScript::ObjectPointer)variable);
        }

        FeebeeCam::settings["label"] = FeebeeCam::_setup->_label,
        FeebeeCam::settings["url"] = FeebeeCam::getURL();

        return result;

    }

    void restartAfterError() {
        std::cerr << "Sleep for " << SLEEP_SECONDS_AFTER_ERROR << " after error" << std::endl;
        nvs_flash_deinit();
        //esp_deep_sleep(SLEEP_SECONDS_AFTER_ERROR * 1000L * 1000L);
        FeebeeCam::light->flash(500, 4);
        ESP.restart();
    }

}