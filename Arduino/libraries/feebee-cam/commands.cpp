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

                    FeebeeCam::initializeWebServer();
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

                case STOP_CAMERA:
                    std::cerr << "Stopping camera" << std::endl;
                    FeebeeCam::stopCamera();
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
            FeebeeCam::commands.push(FeebeeCam::STOP_CAMERA);
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
        else if (command == "save") {
            _setup->save();
            object["status"] = true;
            object["message"] = "Camera saved";
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
                
        
        client->_contentType = "application/json; charset=utf-8";
        client->_chunkedEncoding = true;
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();
        
        stream << object;

        client->sendFinalChunk();

        Serial.print("Sent camera command ");
        std::string _command = command.str();
        Serial.println(_command.c_str());


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


        unsigned long wakeupEvery = 0;
        
        if (settings.contains("wakeupEvery"))
            wakeupEvery = (double)settings["wakeupEvery"];
        
        if (wakeupEvery == 0) {
            wakeupEvery = CHECK_EVERY_SECONDS;
            settings["wakeupEvery"] = (double)wakeupEvery;
        }

        unsigned long sleepTimeMicroSeconds = wakeupEvery * 1000L * 1000L;

        settings["sleeping"] = true;
        settings["wakeup"] = false;
        settings["sleepTime"] = FeebeeCam::getDateTime();

        double epoch = FeebeeCam::getEpoch();

        double wakeupTimeEpoch = epoch + wakeupEvery;

        time_t wakeupTime = static_cast<time_t>(wakeupTimeEpoch);

        settings["wakeupTime"] = FeebeeCam::getDateTime(&wakeupTime);

        settings.save();        
        FeebeeCam::_setup->save();
        
        Serial.print("Putting to sleep for ");
        Serial.print(wakeupEvery);
        Serial.println(" seconds");

        Serial.flush();

        FeebeeCam::weather.sleep();
        FeebeeCam::light->flash(100, 2);


        FeebeeCam::initializeRTC();
        bmm8563_clearIRQ();
        
        //bmm8563_setTimerIRQ(wakeupEvery);
        std::tm* irqWakeupTime = std::localtime(&wakeupTime);
        
        bmm8563_setDateIRQ(
            irqWakeupTime->tm_min,
            irqWakeupTime->tm_hour,
            irqWakeupTime->tm_mday,
            -1
        );

        bat_disable_output();

        esp_sleep_enable_timer_wakeup(sleepTimeMicroSeconds);
        esp_deep_sleep_start();


    }

    bool initializeSettings() {

        FeebeeCam::BeeFishStorage storage("/beehive/");
        BeeFishBScript::Variable variable = storage.getItem("settings");

        bool result = true;

        if (variable == nullptr || variable == undefined) {
            cerr << "Creating default settings" << endl;
            //FeebeeCam::putToSleep();
        }
        else {
            cerr << "Using settings from cloud" << endl;

            FeebeeCam::settings.apply((BeeFishBScript::ObjectPointer)variable);
        }

        if (!FeebeeCam::settings.contains("wakeupEvery"))
            FeebeeCam::settings["wakeupEvery"] = CHECK_EVERY_SECONDS;

        if (!FeebeeCam::settings.contains("takePictureEvery"))
            FeebeeCam::settings["takePictureEvery"] = TAKE_PICTURE_EVERY;

        if (!FeebeeCam::settings.contains("wakeup"))
            FeebeeCam::settings["wakeup"] = true;

        FeebeeCam::settings["label"] = FeebeeCam::_setup->_label,
        FeebeeCam::settings["url"] = FeebeeCam::getURL();

        return result;

    }

    void restartAfterError(const char* file, const char* function, int line) {
        std::cerr << "Error occurred. Restarting." << std::endl;
        std::cerr << file << "[" << line << "]:" << function << endl;
        FeebeeCam::light->flash(500, 4);
        ESP.restart();
    }

}