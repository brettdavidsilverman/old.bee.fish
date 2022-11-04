#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>

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

   //if (FeebeeCam::isInternetInitialized || FeebeeCam::isConnectedToESPAccessPoint) {
      if (FeebeeCam::webServer)
         FeebeeCam::WebServer::loop(FeebeeCam::webServer);

//      if (FeebeeCam::webServer8080)
//         FeebeeCam::WebServer::loop(FeebeeCam::webServer8080);
   
   //}

   if (FeebeeCam::isInternetInitialized)
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

         if (FeebeeCam::initializeTimers()) {
            // Upload weather report with frame buffer
            FeebeeCam::uploadImage();
         }

         // Upload weather report
         FeebeeCam::uploadWeatherReport();

         if (FeebeeCam::status._wakeupNextTime == false) {
            
               // putToSleep saves settings before sleeping
            FeebeeCam::putToSleep();
         }

         FeebeeCam::status._sleeping = false;

         FeebeeCam::status.save();
         //FeebeeCam::_setup->save();
         
         FeebeeCam::light->turnOff();

         FeebeeCam::isInternetInitialized = true;

      }

      cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;

      return true;
   }

}