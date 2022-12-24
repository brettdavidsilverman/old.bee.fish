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
   FeebeeCam::initializeRTC();

   FeebeeCam::initializeCamera();
   FeebeeCam::initializeTime();

   FeebeeCam::initializeCommands();
   FeebeeCam::initializeWiFi();
   FeebeeCam::checkCommandLine();
   FeebeeCam::resetCameraWatchDogTimer();

   //FeebeeCam::downloadFiles(false, true);

}


void loop() {

   // We use custom Command loop in commands.cpp instead of default loop
   // to allow for greater stack size

}


namespace FeebeeCam {


   bool onConnectedToInternet() {

      std::cerr << "Connected to internet" << std::endl;

      FeebeeCam::isConnectedToInternet = true;

      if (!FeebeeCam::isTimeInitialized())
         FeebeeCam::initializeTime();

      if (FeebeeCam::_setup->_isSetup) {

         FeebeeCam::initializeStatus();

         if (FeebeeCam::status._wakeupNextTime == false) {

            FeebeeCam::handleUploads();

            // putToSleep saves settings before sleeping
            //FeebeeCam::putToSleep();
         }

         FeebeeCam::status._sleeping = false;

         FeebeeCam::status.save();

      } 

      FeebeeCam::Light light;
      light.turnOff();
         
      cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;

      return true;
   }

}