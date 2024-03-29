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
#include "setup.h"

bool initializeWebServers();

namespace FeebeeCam {

   Commands commands;

   void Commands::loop(void* param) {

      for (;;) 
      {


         FeebeeCam::handleCommandLine();

         if (FeebeeCam::dnsServer != nullptr) {
            FeebeeCam::dnsServer->processNextRequest();
         }


         //if (!FeebeeCam::isCameraRunning)
         //   FeebeeCam::handleUploads(true);

         if (!commands.empty()) {

            command_t command = commands.pop();

            switch (command) {
            
               case INITIALIZE_WEBSERVER:

                  FeebeeCam::initializeWebServers();
                  break;

               case INTERNET:
                  FeebeeCam::onConnectedToInternet();
                  FeebeeCam::initializeWebServers();
                  break;

               case SAVE_SETUP:
                  std::cerr << "Saving setup" << std::endl;
                  FeebeeCam::_setup->save();
                  break;

               case UPLOAD_WEATHER: {
                  std::cerr << "Upload weather report" << std::endl;
                  BeeFishId::Id id;
                  FeebeeCam::uploadWeatherReport(id);
                  break;


               }
               case PUT_TO_SLEEP:
                  delay(1000);
                  std::cerr << "Put to sleep" << std::endl;
                  FeebeeCam::putToSleep();
                  break;

               case DOWNLOAD_FILES:
               {
                  std::cerr << "Download files" << std::endl;
                  bool downloadBinary = false;
                  bool overrideVersion = true;
                  FeebeeCam::downloadFiles(downloadBinary, overrideVersion);
               }
               break;
               case RESTART:
                  delay(1000);
                  std::cerr << "Restarting now" << std::endl;
                  ESP.restart();
                  break;

               case STOP_CAMERA:
                  delay(1000);
                  std::cerr << "Stopping camera" << std::endl;
                  FeebeeCam::stopCamera();
                  std::cerr << "Camera stopped" << std::endl;
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
         Commands::loop,      // Task function. 
         "commands",          // String with name of task. 
         COMMAND_STACK_SIZE,  // Stack size in bytes. 
         NULL,                // Parameter passed as input of the task 
         0,                   // Priority of the task. 
         &handle,             // Task handle
         COMMAND_CORE         // Pinned to core 
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
         object["statusURL"] = HOST "/download";
         _downloadFiles = true;
      }
            
      
      client->_statusCode = 200;
      client->_statusText = "OK";
      client->_contentType = "application/json; charset=utf-8";
      client->_contentLength = object.contentLength();
      client->sendHeaders();

      BeeFishBString::BStream& stream = client->getOutputStream();

      std::cerr << object << std::endl;

      stream << object;

      client->flush();

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

      // Stop the camera if running
      if (FeebeeCam::isCameraRunning)
         FeebeeCam::stopCamera();
 
      if (FeebeeCam::_setup->_wakeupEvery <= 0.0)
         FeebeeCam::_setup->_wakeupEvery = WAKEUP_EVERY_SECONDS;

      long long seconds = FeebeeCam::_setup->_wakeupEvery;
      FeebeeCam::status._wakeupNextTime = false;

      unsigned long long sleepTimeMicroSeconds = seconds * 1000L * 1000L;

      FeebeeCam::status._sleepTime = FeebeeCam::getDateTime();

      uint64_t epoch = FeebeeCam::getEpoch();

      uint64_t wakeupTimeEpoch = epoch + seconds;

      time_t wakeupTime = static_cast<time_t>(wakeupTimeEpoch);

      FeebeeCam::status._wakeupTime = FeebeeCam::getDateTime(&wakeupTime);
      FeebeeCam::status._sleeping = true;

      if (!FeebeeCam::status.save()) {
         RESTART_AFTER_ERROR("Error saving status");
      }
      
      weather1.sleep();
      weather2.sleep();

      WiFi.disconnect(true);

      light.flash(200, 2);
      light.turnOff();

      cerr 
         << "Putting to sleep for " 
         << seconds
         << " seconds"
         << endl;
          
      cerr.flush();
      
      esp_sleep_enable_timer_wakeup(sleepTimeMicroSeconds);
      esp_deep_sleep_start();

      throw std::runtime_error("Should never reach here");

   }


   void restartAfterError(const char* reason, const char* file, const char* function, int line) {
      std::cerr << reason << std::endl;
      std::cerr << file << "[" << line << "]:" << function << endl;
      light.flash(100, 5);

      delay(1000);
      
      ESP.restart();
   }

}