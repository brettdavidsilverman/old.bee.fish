#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>

namespace FeebeeCam {
   bool internetInitialized = false;
}

void setup() {

   FeebeeCam::initializeSerial();

   FeebeeCam::initializeMemory();
   FeebeeCam::initializeBattery();
   FeebeeCam::initializeFileSystem();
   FeebeeCam::initializeSetup();
   FeebeeCam::initializeCamera(FRAME_BUFFER_COUNT);
   FeebeeCam::initializeTime();
   FeebeeCam::initializeMainBoardTwoWire();
   FeebeeCam::initializeLight();

   FeebeeCam::light->flash(100, 1);

   FeebeeCam::initializeCommands();
   FeebeeCam::initializeWiFi();
   FeebeeCam::checkCommandLine();
   FeebeeCam::resetCameraWatchDogTimer();

   //FeebeeCam::downloadFiles(false, true);

}


void loop() {

   FeebeeCam::handleCommandLine();

   if (FeebeeCam::dnsServer)
      FeebeeCam::dnsServer->processNextRequest();

   if (FeebeeCam::webServer)
      FeebeeCam::WebServer::loop(FeebeeCam::webServer);

   if (FeebeeCam::_setup->_isSetup && FeebeeCam::internetInitialized) {

      FeebeeCam::handleUploads();

      if (millis() >= FeebeeCam::cameraWatchDogTimer) {
         std::cerr << "Camera watch dog triggered" << std::endl;
         FeebeeCam::resetCameraWatchDogTimer();
         FeebeeCam::putToSleep();
      };
   }

   delay(1);

}


namespace FeebeeCam {


   bool onConnectedToInternet() {

      FeebeeCam::internetInitialized = false;

      if (!FeebeeCam::isTimeInitialized())
         FeebeeCam::initializeTime();

      if (FeebeeCam::_setup->_isSetup) {

         FeebeeCam::initializeStatus();

         if (FeebeeCam::status._wakeupNextTime == false) {

            FeebeeCam::handleUploads();

            // putToSleep saves settings before sleeping
            FeebeeCam::putToSleep();
         }

         FeebeeCam::status._sleeping = false;

         FeebeeCam::status.save();
         
         FeebeeCam::light->turnOff();

      }

      cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;
      
      FeebeeCam::internetInitialized = true;

      return true;
   }

}