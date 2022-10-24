#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>


void setup() {

   FeebeeCam::initializeSerial();

   FeebeeCam::initializeMemory();
   //FeebeeCam::initializeCamera(FRAME_BUFFER_COUNT);
   FeebeeCam::initializeBattery();
   FeebeeCam::initializeMultiplexer();
   FeebeeCam::initializeLight();

   FeebeeCam::light->flash(100, 1);

   FeebeeCam::initializeCommands();
   FeebeeCam::initializeFileSystem();
   FeebeeCam::initializeSetup();
   FeebeeCam::initializeWiFi();
   //FeebeeCam::checkCommandLine();
   FeebeeCam::resetCameraWatchDogTimer();
}


void loop() {

   FeebeeCam::handleCommandLine();
   FeebeeCam::handleCommands();

   /*
      if (FeebeeCam::webServer)
            FeebeeCam::webServer->loop();

      if (FeebeeCam::cameraWebServer)
            FeebeeCam::cameraWebServer->loop();
*/
   if (FeebeeCam::dnsServer)
      FeebeeCam::dnsServer->processNextRequest();

   if (millis() > FeebeeCam::cameraWatchDogTimer) {
      std::cerr << "Camera watch dog triggered" << std::endl;
      std::cerr << "SKIPPING CAMERA WATCH DOG" << std::endl;
      FeebeeCam::resetCameraWatchDogTimer();
      //FeebeeCam::putToSleep();
   };

   vTaskDelay(5);
}


namespace FeebeeCam {

bool onConnectedToInternet() {

   cerr << "Connected to internet" << endl;

   //FeebeeCam::initializeRTC();

   //FeebeeCam::downloadFiles(false, true);

   // Reinitialize the multiplexer after accessing rtc wire
   //FeebeeCam::initializeMultiplexer();
   //FeebeeCam::initializeLight();

   if (FeebeeCam::_setup->_isSetup) {

      FeebeeCam::initializeTime();

      //                  if (!FeebeeCam::BeeFishWebRequest::logon(FeebeeCam::_setup->_secretHash))
      //                        RESTART_AFTER_ERROR();

      FeebeeCam::initializeSettings();

      unsigned long takePictureEvery;

      if (!settings.contains("takePictureEvery"))
         settings["takePictureEvery"] = TAKE_PICTURE_EVERY;

      takePictureEvery =
         (double)settings["takePictureEvery"];


      bool takePicture = false;
      time_t lastImageTimeEpoch;
      if (!settings.contains("lastImageTime"))
         takePicture = true;
      else {
         BString lastImageTime = settings["lastImageTime"];

         std::tm _lastImageTime;
         std::string strLastImageTime = lastImageTime.str();
         std::stringstream stream(strLastImageTime.c_str());
         //23 Sep 2022 17:28:51
         //%d %b %Y %H:%M:%S
         stream >> std::get_time(&_lastImageTime, "%d %b %Y %H:%M:%S");
         lastImageTimeEpoch = mktime(&_lastImageTime);
      }

      double epoch = FeebeeCam::getEpoch();

      if (takePicture || (lastImageTimeEpoch + takePictureEvery < epoch)) {
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

      FeebeeCam::light->turnOff();
   }

   cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;

   return true;
}

}