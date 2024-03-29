#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>

void setup() {

   FeebeeCam::initializeSerial();
   FeebeeCam::initializeBattery();
   FeebeeCam::initializeMemory();
   FeebeeCam::initializeCamera(FRAME_BUFFER_COUNT);
 

   FeebeeCam::initializeFileSystem();
   FeebeeCam::initializeSetup();
   FeebeeCam::initializeWiFi();
   

   //FeebeeCam::_setup->_isSetup = false;

   FeebeeCam::initializeLight();
   FeebeeCam::initializeWeather();

   FeebeeCam::initializeCommands();
   
   std::cerr << "Enter command or ignore to continue..." << endl;
   delay(1000);

   
   FeebeeCam::resetCameraWatchDogTimer();

   //FeebeeCam::downloadFiles(false, true);

}


void loop() {

   // We use custom Command loop in commands.cpp instead of default loop
   // to allow for greater stack size

   if ( FeebeeCam::_setup->_isSetup && 
        millis() >= FeebeeCam::cameraWatchDogTimer) 
   {
      FeebeeCam::resetCameraWatchDogTimer();
      RESTART_AFTER_ERROR("Camera Watch Dog");
   };

   delay(1000);
   
}


namespace FeebeeCam {


   bool onConnectedToInternet() {

      FeebeeCam::isConnectedToInternet = true;
      
      if ( !FeebeeCam::_setup->_isSetup ) 
         return false;

      if (!FeebeeCam::downloadFiles(true, false))
         return false;

      FeebeeCam::resetCameraWatchDogTimer();

      if (!FeebeeCam::initializeTime())
         return false;

      if (!FeebeeCam::initializeStatus())
         return false;
 
      FeebeeCam::status._url = FeebeeCam::getURL();
      
      if (!FeebeeCam::status._wakeupNextTime) {

         FeebeeCam::handleUploads(false);

         // putToSleep saves settings before sleeping
         FeebeeCam::putToSleep();

         throw std::runtime_error("Should never reach here");
      }
      
      FeebeeCam::status._sleeping = false;

      return FeebeeCam::status.save();

   }

}