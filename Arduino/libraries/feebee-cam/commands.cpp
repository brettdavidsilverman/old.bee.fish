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
#include "web-server.h"
#include "serial.h"
#include "wifi.h"

namespace FeebeeCam {

    Commands commands;

    void Commands::loop(void* param) {

        for (;;) {

            delay(1);

            FeebeeCam::handleCommandLine();

            if (FeebeeCam::dnsServer)
                FeebeeCam::dnsServer->processNextRequest();

            if (FeebeeCam::webServer)
                FeebeeCam::WebServer::loop(FeebeeCam::webServer);

            if (FeebeeCam::webServerCamera)
                FeebeeCam::WebServer::loop(FeebeeCam::webServerCamera);

            if (FeebeeCam::isConnectedToInternet) {
                if (FeebeeCam::handleUploads()) {
                    FeebeeCam::status.save();
                }
            }

            if (millis() >= FeebeeCam::cameraWatchDogTimer) {
                std::cerr << "Camera watch dog triggered" << std::endl;
                FeebeeCam::resetCameraWatchDogTimer();
                FeebeeCam::commands.push(FeebeeCam::PUT_TO_SLEEP);
            };

            if (!commands.empty()) {

                command_t command = commands.pop();

                switch (command) {
                
                    case INTERNET:
                        FeebeeCam::onConnectedToInternet();
                        break;

                    case INITIALIZE_WEBSERVER:
                        FeebeeCam::initializeWebServers();
                        break;

                    case SAVE_SETUP:
                        std::cerr << "Saving setup" << std::endl;
                        FeebeeCam::_setup->save();
                        break;

                    case UPLOAD_WEATHER:
                        std::cerr << "Upload weather report" << std::endl;
                        FeebeeCam::uploadWeatherReport();
                        break;

                    case PUT_TO_SLEEP:
                        delay(1000);
                        std::cerr << "Put to sleep" << std::endl;
                        FeebeeCam::putToSleep();
                        break;

                    case DOWNLOAD_FILES:
                        std::cerr << "Download files" << std::endl;
                        FeebeeCam::downloadFiles(true, true);
                        break;

                    case RESTART:
                        delay(1000);
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
    }

    bool initializeCommands() {

        TaskHandle_t handle = nullptr;

        xTaskCreatePinnedToCore(
            Commands::loop,   // Task function. 
            "commands",           // String with name of task. 
            20000,                       // Stack size in bytes. 
            NULL,                  // Parameter passed as input of the task 
            0,                          // Priority of the task. 
            &handle,                    // Task handle
            COMMAND_CORE                           // Pinned to core 
        );

        return handle != nullptr;
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
            FeebeeCam::stopCamera();
            //FeebeeCam::commands.push(FeebeeCam::STOP_CAMERA);
            object["status"] = true;
            object["message"] = "Camera stopped";
        }
        else if (command == "reset") {
            _setup->reset();
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

        cerr << "Sent camera command " << command << "..." << flush;

        if (_putToSleep) {
            FeebeeCam::commands.push(FeebeeCam::PUT_TO_SLEEP);
        }

        if (_downloadFiles) {
            FeebeeCam::commands.push(FeebeeCam::DOWNLOAD_FILES);
        }

        if (restart) {
            FeebeeCam::commands.push(FeebeeCam::RESTART);
        }

        cerr << "Ok" << endl;
        
        return true;

    }

    bool putToSleep() {

        using namespace std;

        FeebeeCam::Light light;
        
        if (!FeebeeCam::_setup->_isSetup)
            return false;

        // Stop the camera if running
        if (FeebeeCam::isCameraRunning)
            FeebeeCam::stopCamera();
 
        if (status._wakeupEvery <= 0.0)
            status._wakeupEvery = WAKEUP_EVERY_SECONDS;

        unsigned long sleepTimeMicroSeconds = status._wakeupEvery * 1000L * 1000L;

        status._sleeping = true;
        status._wakeupNextTime   = false;
        status._sleepTime = FeebeeCam::getDateTime();

        uint64_t epoch = FeebeeCam::getEpoch();

        uint64_t wakeupTimeEpoch = epoch + status._wakeupEvery;

        time_t wakeupTime = static_cast<time_t>(wakeupTimeEpoch);

        status._wakeupTime = FeebeeCam::getDateTime(&wakeupTime);

        if (FeebeeCam::isConnectedToInternet)
            FeebeeCam::status.save();
        
        FeebeeCam::Weather weather1(1);
        FeebeeCam::Weather weather2(2);

        weather1.sleep();
        weather2.sleep();

        WiFi.disconnect(true);

        light.turnOff();

        FeebeeCam::initializeRTC();
        
        //bmm8563_setTimerIRQ(wakeupEvery);
        std::tm* irqWakeupTime = std::localtime(&wakeupTime);
        
        bmm8563_setDateIRQ(
            irqWakeupTime->tm_min,
            irqWakeupTime->tm_hour,
            irqWakeupTime->tm_mday,
            -1
        );

        cerr 
            << "Putting to sleep for " 
            << status._wakeupEvery 
            << " seconds"
            << endl;
             
        bat_disable_output();

        esp_sleep_enable_timer_wakeup(sleepTimeMicroSeconds);
        esp_deep_sleep_start();


    }


    void restartAfterError(const char* file, const char* function, int line) {
        std::cerr << "Error occurred." << std::endl;
        std::cerr << file << "[" << line << "]:" << function << endl;
        FeebeeCam::Light light;
        light.flash(100, 5);
        if (FeebeeCam::_setup->_isSetup)
            FeebeeCam::putToSleep();
        else
            ESP.restart();
    }

}