#include <feebee-cam.h>
#include <iostream>

namespace FeebeeCam {

   using namespace std;

   bool handleUploads(bool updateStatus) {

      return false;
      
      uint64_t milliSeconds = millis();

      static uint64_t checkTimers = 0;
      static uint64_t nextUploadWeatherTime = 0;

      if (milliSeconds < checkTimers)
         return false;

      if (!FeebeeCam::_setup->_isSetup)
         return false;

      bool dataUploaded = false;

      if ( milliSeconds >= nextUploadWeatherTime ) {
         nextUploadWeatherTime = milliSeconds + FeebeeCam::_setup->_wakeupEvery * 1000;

         if (FeebeeCam::isCameraRunning && !FeebeeCam::isPaused)
               FeebeeCam::pauseCamera();
         
         if (FeebeeCam::uploadWeatherReport()) {
            dataUploaded = true;
            cerr << "Weather uploaded" << endl;
         }
         else {
            cerr << "Error uploading weather" << endl;
         }
      }  

      if (FeebeeCam::initializeTimers()) {

         if (FeebeeCam::isCameraRunning && !FeebeeCam::isPaused)
               FeebeeCam::pauseCamera();

         if (FeebeeCam::uploadImage()) {
            cerr << "Image uploaded" << endl;
            dataUploaded = true;
         }
         else
            cerr << "Error uploading image" << endl;
      }

      if (dataUploaded && updateStatus) {
         FeebeeCam::status.save();
      }

      if (FeebeeCam::isPaused)
         FeebeeCam::resumeCamera();

      checkTimers = milliSeconds + 5000;

      return dataUploaded;
   }
   
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
         FeebeeCam::_setup->_takePictureEvery;

      int64_t epoch = FeebeeCam::getEpoch();

//      cerr << "*********************" << endl;
//      cerr << "takeNextPictureTime: " << takeNextPictureTime << endl;
//      cerr << "lastImageTimeEpoch:  " << lastImageTimeEpoch << endl;
//      cerr << "difference:          " << takeNextPictureTime - lastImageTimeEpoch << endl;
//      cerr << "epoch:               " << epoch << endl;

      if (epoch >= takeNextPictureTime)
         return true;
      else
         return false;

   }
}
