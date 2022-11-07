#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>

void setup() {

   FeebeeCam::initializeSerial();

   FeebeeCam::initializeMemory();
   FeebeeCam::initializeBattery();
   FeebeeCam::initializeMainBoardTwoWire();
   FeebeeCam::initializeFileSystem();
   FeebeeCam::initializeSetup();
   FeebeeCam::initializeCamera(FRAME_BUFFER_COUNT);
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

   if (FeebeeCam::dnsServer)
      FeebeeCam::dnsServer->processNextRequest();

   if (FeebeeCam::webServer)
      FeebeeCam::WebServer::loop(FeebeeCam::webServer);

   FeebeeCam::handleUploads();

   uint64_t milliSeconds = millis();

   if (milliSeconds >= FeebeeCam::cameraWatchDogTimer) {
      std::cerr << "Camera watch dog triggered" << std::endl;
      FeebeeCam::resetCameraWatchDogTimer();
      FeebeeCam::putToSleep();
   };

   delay(10);

}


namespace FeebeeCam {


   bool onConnectedToInternet() {

      //FeebeeCam::downloadFiles(false, true);
      FeebeeCam::isInternetInitialized = false;
         
      if (!FeebeeCam::isTimeInitialized())
         FeebeeCam::initializeTime();

      if (FeebeeCam::_setup->_isSetup) {

         FeebeeCam::initializeStatus();

         FeebeeCam::handleUploads();

         if (FeebeeCam::status._wakeupNextTime == false) {
            
               // putToSleep saves settings before sleeping
            FeebeeCam::putToSleep();
         }

         FeebeeCam::status._sleeping = false;

         FeebeeCam::status.save();
         
         FeebeeCam::light->turnOff();

         FeebeeCam::isInternetInitialized = true;

      }

      cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;

      return true;
   }

}