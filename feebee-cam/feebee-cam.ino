#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>

namespace FeebeeCam {
   bool initializeTimers();
}

void setup() {

   FeebeeCam::initializeSerial();

   FeebeeCam::initializeMemory();
   //FeebeeCam::initializeCamera(FRAME_BUFFER_COUNT);
   FeebeeCam::initializeBattery();
   FeebeeCam::initializeMultiplexer();
   FeebeeCam::initializeFileSystem();
   FeebeeCam::initializeSetup();
   FeebeeCam::initializeTime();
   FeebeeCam::initializeLight();

   FeebeeCam::light->flash(100, 1);

   FeebeeCam::initializeCommands();
   FeebeeCam::initializeWiFi();
   //FeebeeCam::checkCommandLine();
   FeebeeCam::resetCameraWatchDogTimer();
}


void loop() {

   FeebeeCam::handleCommandLine();
   FeebeeCam::handleCommands();

   if (FeebeeCam::dnsServer)
      FeebeeCam::dnsServer->processNextRequest();

   if (FeebeeCam::webServer80)
      FeebeeCam::WebServer::loop(FeebeeCam::webServer80);

   if (FeebeeCam::webServer8080)
      FeebeeCam::WebServer::loop(FeebeeCam::webServer8080);

   static uint64_t checkTimers = 0;

   if (  FeebeeCam::_setup->_isSetup  &&
         ( millis() >= checkTimers ) )
   {

      if (FeebeeCam::initializeTimers()) {
         if (FeebeeCam::uploadImage())
            cerr << "Image uploaded" << endl;
         else
            cerr << "Error uploading image" << endl;
      }

      checkTimers = millis() + 1000;
   }

   if (millis() >= FeebeeCam::cameraWatchDogTimer) {
      std::cerr << "Camera watch dog triggered" << std::endl;
      FeebeeCam::resetCameraWatchDogTimer();
      FeebeeCam::putToSleep();
   };

   delay(1);

}


namespace FeebeeCam {

   bool initializeTimers() {
      
      int64_t takeNextPictureTime = 0;
      int64_t lastImageTimeEpoch;
      double takePictureEvery;

      if (settings.contains("lastImageTime"))
      {
         BString strlastImageTime = settings["lastImageTime"];
         std::tm lastImageTime;

         std::stringstream stream(strlastImageTime.str());
         
         // 23 Sep 2022 17:28:51
         // %d %b  %Y   %H:%M:%S
         stream >> std::get_time(&lastImageTime, "%d %b %Y %H:%M:%S");
         lastImageTimeEpoch = mktime(&lastImageTime);
      }
      else {
         lastImageTimeEpoch = 0;
      }

      if (!settings.contains("takePictureEvery"))
         settings["takePictureEvery"] = TAKE_PICTURE_EVERY;

      takePictureEvery = settings["takePictureEvery"];

      takeNextPictureTime = 
         lastImageTimeEpoch + 
         takePictureEvery;

      int64_t epoch = FeebeeCam::getEpoch();
/*
      cerr << "takeNextPictureTime: " << takeNextPictureTime << endl;
      cerr << "lastImageTimeEpoch:  " << lastImageTimeEpoch << endl;
      cerr << "difference:          " << takeNextPictureTime - lastImageTimeEpoch << endl;
      cerr << "epoch:               " << epoch << endl;
*/
      if (epoch >= takeNextPictureTime)
         return true;
      else
         return false;

   }

   bool onConnectedToInternet() {

      cerr << "Connected to internet" << endl;

      //FeebeeCam::downloadFiles(false, true);
         
      if (!FeebeeCam::isTimeInitialized())
         FeebeeCam::initializeTime();

      if (FeebeeCam::_setup->_isSetup) {

         FeebeeCam::initializeSettings();

         if (FeebeeCam::initializeTimers()) {
            // Upload weather report with frame buffer
            FeebeeCam::uploadImage();
         }

         // Upload weather report
         FeebeeCam::uploadWeatherReport();

         if (FeebeeCam::settings.contains("wakeup") && !FeebeeCam::settings["wakeup"]) {
            // if successfull, put back to sleep
            // putToSleep saves settings before sleeping
            FeebeeCam::putToSleep();
         }

         FeebeeCam::settings["sleeping"] = false;

         FeebeeCam::settings.save();
         FeebeeCam::_setup->save();
         
         FeebeeCam::light->turnOff();

         
      }

      cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;

      return true;
   }

}