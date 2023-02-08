#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>

void setup() {

   FeebeeCam::initializeSerial();
   FeebeeCam::initializeBattery();

   FeebeeCam::initializeMemory();

   FeebeeCam::initializeFileSystem();
   FeebeeCam::initializeSetup();
   
   //FeebeeCam::_setup->_isSetup = false;

   FeebeeCam::initializeLight();
   FeebeeCam::initializeCamera();
   FeebeeCam::initializeWeather();

   FeebeeCam::initializeCommands();
   
   std::cerr << "Enter command or ignore to continue..." << endl;
   delay(1000);

   FeebeeCam::initializeWiFi();
   FeebeeCam::resetCameraWatchDogTimer();

   //FeebeeCam::downloadFiles(false, true);

}


void loop() {

   // We use custom Command loop in commands.cpp instead of default loop
   // to allow for greater stack size

   if ( FeebeeCam::_setup->_isSetup && 
        millis() >= FeebeeCam::cameraWatchDogTimer) 
   {
      std::cerr << "Camera watch dog triggered" << std::endl;
      FeebeeCam::resetCameraWatchDogTimer();
      
      RESTART_AFTER_ERROR();
   };

   delay(1000);
   
}


namespace FeebeeCam {


   bool onConnectedToInternet() {

      FeebeeCam::isConnectedToInternet = true;

      if ( FeebeeCam::_setup->_isSetup ) 
      {

         if (!FeebeeCam::initializeTime())
            return false;

         if (!FeebeeCam::initializeStatus())
            return false;

         FeebeeCam::status._sleeping = true;

         if (FeebeeCam::status._wakeupNextTime == false) {

            //FeebeeCam::handleUploads(false);
            //FeebeeCam::uploadImage();
            if ( FeebeeCam::isConnectedToInternet &&
               FeebeeCam::_setup->_isSetup )
            {
               BeeFishId::Id urlId;

               if (FeebeeCam::uploadImage(urlId))
                  FeebeeCam::uploadWeatherReport(urlId);

            }

            // putToSleep saves settings before sleeping
            FeebeeCam::putToSleep();

            throw std::runtime_error("Should never reach here");
         }

         FeebeeCam::status._sleeping = false;

         FeebeeCam::status.save();

      } 


      return true;
   }

}