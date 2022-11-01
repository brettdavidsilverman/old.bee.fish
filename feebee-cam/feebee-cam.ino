#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>

namespace FeebeeCam {
   bool initializeTimers();
   bool internetInitialized = false;
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
   //FeebeeCam::handleCommands();

   if (FeebeeCam::dnsServer)
      FeebeeCam::dnsServer->processNextRequest();

   if (FeebeeCam::webServer80)
      FeebeeCam::WebServer::loop(FeebeeCam::webServer80);

   if (FeebeeCam::webServer8080)
      FeebeeCam::WebServer::loop(FeebeeCam::webServer8080);

   uint64_t milliSeconds = millis();

   static uint64_t checkTimers = 0;
   static uint64_t nextUploadWeatherTime = 0;

   if (FeebeeCam::internetInitialized && FeebeeCam::_setup->_isSetup ) {

      if ( milliSeconds >= nextUploadWeatherTime ) {
         nextUploadWeatherTime = milliSeconds + FeebeeCam::status._wakeupEvery * 1000;
         if (FeebeeCam::uploadWeatherReport())
            cerr << "Uploaded weather report" << endl;
         else
            cerr << "Error uploading weather report" << endl;
      }  

      if ( milliSeconds >= checkTimers )
      {

         if (FeebeeCam::initializeTimers()) {
            if (FeebeeCam::uploadImage())
               cerr << "Image uploaded" << endl;
            else
               cerr << "Error uploading image" << endl;
         }

         checkTimers = milliSeconds + 1000;
      }
   }

   if (milliSeconds >= FeebeeCam::cameraWatchDogTimer) {
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

      if (status._lastImageTime.length())
      {
         std::tm lastImageTime;
         std::stringstream stream(status._lastImageTime.str());
         
         // 23 Sep 2022 17:28:51
         // %d %b  %Y   %H:%M:%S
         stream >> std::get_time(&lastImageTime, "%d %b %Y %H:%M:%S");
         lastImageTimeEpoch = mktime(&lastImageTime);
      }
      else {
         lastImageTimeEpoch = 0;
      }

      takeNextPictureTime = 
         lastImageTimeEpoch + 
         status._takePictureEvery;

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

      //FeebeeCam::downloadFiles(false, true);
         
      if (!FeebeeCam::isTimeInitialized())
         FeebeeCam::initializeTime();

      if (FeebeeCam::_setup->_isSetup) {

         FeebeeCam::initializeStatus();

         if (FeebeeCam::status._wakeupNextTime == false) {
            
            if (FeebeeCam::initializeTimers()) {
               // Upload weather report with frame buffer
               FeebeeCam::uploadImage();
            }

            // Upload weather report
            FeebeeCam::uploadWeatherReport();

               // putToSleep saves settings before sleeping
            FeebeeCam::putToSleep();
         }

         FeebeeCam::status._sleeping = false;

         FeebeeCam::status.save();
         //FeebeeCam::_setup->save();
         
         FeebeeCam::light->turnOff();

         FeebeeCam::internetInitialized = true;

      }

      cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;

      return true;
   }

}