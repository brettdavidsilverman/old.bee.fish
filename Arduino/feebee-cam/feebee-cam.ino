#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>

void setup() {

   FeebeeCam::initializeSerial();
   FeebeeCam::initializeBattery();

   FeebeeCam::initializeMemory();

   FeebeeCam::initializeFileSystem();
   FeebeeCam::initializeSetup();

   FeebeeCam::initializeLight();
   FeebeeCam::initializeCamera();
   FeebeeCam::initializeWeather();

   FeebeeCam::initializeCommands();
   FeebeeCam::initializeWiFi();
   FeebeeCam::checkCommandLine();
   FeebeeCam::resetCameraWatchDogTimer();

   //FeebeeCam::downloadFiles(false, true);

}


void loop() {

   // We use custom Command loop in commands.cpp instead of default loop
   // to allow for greater stack size

   if (millis() >= FeebeeCam::cameraWatchDogTimer) {
      std::cerr << "Camera watch dog triggered" << std::endl;
      FeebeeCam::resetCameraWatchDogTimer();
      RESTART_AFTER_ERROR();
   };


}


namespace FeebeeCam {


   bool onConnectedToInternet() {

      std::cerr << "Connected to internet" << std::endl;

      FeebeeCam::isConnectedToInternet = true;

      FeebeeCam::initializeTime();

      if (FeebeeCam::_setup->_isSetup) {

         FeebeeCam::initializeStatus();

         if (FeebeeCam::status._wakeupNextTime == false) {

            FeebeeCam::handleUploads(false);

            // putToSleep saves settings before sleeping
            FeebeeCam::putToSleep();

            throw std::runtime_error("Should never reach here");
         }

         FeebeeCam::status._sleeping = false;

         FeebeeCam::status.save();

      } 

      cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;

      return true;
   }

}