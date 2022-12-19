#include <feebee-cam.h>
#include <iostream>

namespace FeebeeCam {

   using namespace std;
   
   bool handleUploads() {
      
      uint64_t milliSeconds = millis();

      static uint64_t checkTimers = 0;
      static uint64_t nextUploadWeatherTime = 0;

      if (milliSeconds < checkTimers)
         return false;

      if (!FeebeeCam::_setup->_isSetup)
         return false;

      bool dataUploaded = false;

      if (FeebeeCam::initializeTimers()) {
         if (FeebeeCam::uploadImage()) {
            cerr << "Image uploaded" << endl;
            dataUploaded = true;
         }
         else
            cerr << "Error uploading image" << endl;
      }

      if ( milliSeconds >= nextUploadWeatherTime ) {
         nextUploadWeatherTime = milliSeconds + FeebeeCam::status._wakeupEvery * 1000;
         if (FeebeeCam::uploadWeatherReport()) {
            dataUploaded = true;
            cerr << "Uploaded weather report" << endl;
         }
         else {
            cerr << "Error uploading weather report" << endl;
         }
      }  

      if (dataUploaded) {
         FeebeeCam::status.save();
      }

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
}