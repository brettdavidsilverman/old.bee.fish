#include <Arduino.h>
#include <queue>
#include <mutex>
#include "camera.h"
#include "light.h"
#include "setup.h"
#include "commands.h"
#include "weather.h"
#include "commands.h"
#include "local-time.h"
#include "web-storage.h"
#include "settings.h"

#define TAG "Camera"


namespace FeebeeCam {

   bool pause = false;
   bool isPaused = false;
   bool stop = false;
   bool isCameraRunning = false;
   float framesPerSecond = 0.0;
   int  frameCount = 0;
   int64_t lastTimeFramesCounted = 0;
   int64_t cameraWatchDogTimer = 0;

   RTC_DATA_ATTR unsigned long lastTimePictureTaken = 0;
   
   //const uint8_t highQuality = 5;

   bool cameraInitialized = false;

   #define PART_BOUNDARY "123456789000000000000987654321"
   #define _STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
   #define _STREAM_BOUNDARY "\r\n--" PART_BOUNDARY "\r\n"
   #define _STREAM_PART  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"

   bool flushFrameBuffer() {
      // Flush frame buffer;
      for (int i = 0; i < FRAME_BUFFER_COUNT; ++i) {
         camera_fb_t* frameBuffer = esp_camera_fb_get();
         if (frameBuffer)
            esp_camera_fb_return(frameBuffer);
      }

      return true;
   }

   bool initializeCamera(size_t frameBufferCount)
   {
      Serial.println("Initializing camera");
      
      if (cameraInitialized) {
         return true;
         Serial.println("Deinitializing camera");
         esp_camera_deinit();
      }

      cameraInitialized = false;

      camera_config_t camera_config = {
         .pin_pwdn = -1,
         .pin_reset = CAM_PIN_RESET,
         .pin_xclk = CAM_PIN_XCLK,
         .pin_sscb_sda = CAM_PIN_SIOD,
         .pin_sscb_scl = CAM_PIN_SIOC,

         .pin_d7 = CAM_PIN_D7,
         .pin_d6 = CAM_PIN_D6,
         .pin_d5 = CAM_PIN_D5,
         .pin_d4 = CAM_PIN_D4,
         .pin_d3 = CAM_PIN_D3,
         .pin_d2 = CAM_PIN_D2,
         .pin_d1 = CAM_PIN_D1,
         .pin_d0 = CAM_PIN_D0,
         .pin_vsync = CAM_PIN_VSYNC,
         .pin_href = CAM_PIN_HREF,
         .pin_pclk = CAM_PIN_PCLK,

         //XCLK 20MHz or 10MHz
         .xclk_freq_hz = CAM_XCLK_FREQ,
         .ledc_timer = LEDC_TIMER_0,
         .ledc_channel = LEDC_CHANNEL_0,

         .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
         .frame_size = FRAMESIZE_UXGA, 

         .jpeg_quality = 10, //0-63 lower number means higher quality
         .fb_count = frameBufferCount,   //if more than one, i2s runs in continuous mode. Use only with JPEG
         .fb_location = CAMERA_FB_IN_PSRAM,
         .grab_mode = CAMERA_GRAB_WHEN_EMPTY
      };

      esp_err_t ret = esp_camera_init(&camera_config);

      if (ret != ESP_OK) {
         Serial.println("Error initializing camera");
      }

      FeebeeCam::resetCameraWatchDogTimer();
      
      cameraInitialized = true;

      Serial.println("Camera Initialized");

      return true;

   }

   bool stopCamera() {

      if (FeebeeCam::isCameraRunning) {
         FeebeeCam::stop = true;
         while  (FeebeeCam::isCameraRunning)
            vTaskDelay(5);

      }
      FeebeeCam::stop = false;

      return true;
   }

   bool pauseCamera() {

      if (FeebeeCam::isCameraRunning) {

         FeebeeCam::isPaused = false;
         FeebeeCam::pause = true;

         while (!FeebeeCam::isPaused)
            vTaskDelay(5);

      }

      return true;

   }

   bool onCamera(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
      
      Serial.println("Camera");

      if (!FeebeeCam::stopCamera())
         return false;

      if (!FeebeeCam::initializeCamera(FRAME_BUFFER_COUNT))
         return false;
      
      camera_fb_t * frameBuffer = NULL;
      esp_err_t res = ESP_OK;
      size_t _jpg_buf_len;
      uint8_t * _jpg_buf;
      char * part_buf[64];
      
      Data streamBoundary((byte*)_STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));

      client->_contentType = _STREAM_CONTENT_TYPE;

      if (!client->sendHeaders())
         return false;
      
      Serial.println("Starting camera loop");

      FeebeeCam::_setup->applyToCamera();

      bool error = false;

      // Turn on RED
      FeebeeCam::light->turnOn();

      FeebeeCam::isCameraRunning = true;

      while(!error && !FeebeeCam::stop) {

         frameBuffer = esp_camera_fb_get();

         if (!frameBuffer) {
            cerr << "Camera capture failed" << endl;
            break;
         } 

         const Data capturedFrame(frameBuffer->buf, frameBuffer->len);

         size_t headerLength = snprintf((char *)part_buf, 64, _STREAM_PART, capturedFrame.size());

         if (!client->sendChunk(Data((byte*)part_buf, headerLength))) {
            error = true;
            break;
         }

         if (!client->sendChunk(capturedFrame)) {
            error = true;
            break;
         }

         if (!client->sendChunk(streamBoundary)) {
            error = true;
            break;
         }

         esp_camera_fb_return(frameBuffer);
         frameBuffer = nullptr;

         ++FeebeeCam::frameCount;

         int64_t frameEndTime = esp_timer_get_time();
         int64_t frameTime = frameEndTime - FeebeeCam::lastTimeFramesCounted;
         FeebeeCam::framesPerSecond =
            1000.00 * 1000.00 * (float)FeebeeCam::frameCount / (float)frameTime;

         if (FeebeeCam::pause) {

            Serial.println("Paused");
            FeebeeCam::isPaused = true;

            while (FeebeeCam::pause) {
               vTaskDelay(5);
            }

         }

         if (FeebeeCam::isPaused) {

            Serial.println("Resuming");
            FeebeeCam::isPaused = false;

            // Turn on RED
            FeebeeCam::light->turnOn();         

         }

         FeebeeCam::resetCameraWatchDogTimer();

         vTaskDelay(5);


      }

      FeebeeCam::light->turnOff();

      if (frameBuffer)
         esp_camera_fb_return(frameBuffer);


      if (!error) {
         if (!client->sendFinalChunk())
            error = true;
      }
      
      Serial.println("Camera loop ended");

      FeebeeCam::stop = false;
      FeebeeCam::isPaused = false;
      FeebeeCam::pause = false;
      FeebeeCam::isCameraRunning = false;

      return true;

   }

   bool onCapture(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      // Set pause flag to initiate stop camera stream procecss
      
      if (!FeebeeCam::pauseCamera())
         return false;

      if (!FeebeeCam::cameraInitialized) {
         FeebeeCam::initializeCamera(1);
      }


      // Set capture specific settings...

      sensor_t *sensor = esp_camera_sensor_get();

      // Largest frame size?
/*
   FRAMESIZE_96X96,   // 96x96
   FRAMESIZE_QQVGA,   // 160x120
   FRAMESIZE_QCIF,    // 176x144
   FRAMESIZE_HQVGA,   // 240x176
   FRAMESIZE_240X240,  // 240x240
   FRAMESIZE_QVGA,    // 320x240
   FRAMESIZE_CIF,     // 400x296
   FRAMESIZE_HVGA,    // 480x320
   FRAMESIZE_VGA,     // 640x480
   FRAMESIZE_SVGA,    // 800x600
   FRAMESIZE_XGA,     // 1024x768
   FRAMESIZE_HD,      // 1280x720
   FRAMESIZE_SXGA,    // 1280x1024
   FRAMESIZE_UXGA,    // 1600x1200
   // 3MP Sensors
   FRAMESIZE_FHD,     // 1920x1080
   FRAMESIZE_P_HD,    //  720x1280
   FRAMESIZE_P_3MP,   //  864x1536
   FRAMESIZE_QXGA,    // 2048x1536
   // 5MP Sensors
   FRAMESIZE_QHD,     // 2560x1440
   FRAMESIZE_WQXGA,   // 2560x1600
   FRAMESIZE_P_FHD,   // 1080x1920
   FRAMESIZE_QSXGA,   // 2560x1920
   FRAMESIZE_INVALID
*/

      //sensor->set_framesize(sensor, FRAMESIZE_HD);
      // Set framesize to (very) large      
      sensor->set_framesize(sensor, FRAMESIZE_UXGA);

      // Set highest quality
      //sensor->set_quality(sensor, highQuality);

      flushFrameBuffer();
      

      // Set lights on
      light->flashOn();
      light->turnOn();

      // Flush frame buffer, and get the new frame
      camera_fb_t* frameBuffer = esp_camera_fb_get();
      
      // Turn light off
      light->flashOff();
      light->turnOff();

      BeeFishBString::BStream& output = client->getOutputStream();

      if (frameBuffer) {

         const BeeFishBString::Data image(frameBuffer->buf, frameBuffer->len);

         output <<
            "HTTP/1.1 200 OK\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Access-Control-Allow-Origin: null\r\n" <<
            "Cache-Control: no-store, max-age=0\r\n" <<
            "Content-Type: image/jpeg\r\n" <<
            "Content-Length: " << image.size() << "\r\n" <<
            "\r\n";

         output.write((const char *)image._data, image.size());

         esp_camera_fb_return(frameBuffer);

         FeebeeCam::resetCameraWatchDogTimer();
      } 
      else {

         Serial.println("Camera capture failed");

         output << 
            "HTTP/1.1 500 Error\r\n" <<
            "Connection: keep-alive\r\n" <<
            "Access-Control-Allow-Origin: null\r\n" <<
            "Cache-Control: no-store, max-age=0\r\n" <<
            "Content-Type: text/plain\r\n" <<
            "\r\n" <<
            "{\"error\": \"Error with camera capturing frame\"}\r\n";
      }
      
      output.flush();

      // Restore settings and flush frame buffer
      FeebeeCam::_setup->applyToCamera();

      flushFrameBuffer();

      FeebeeCam::pause = false;

      return true;
   }
 
   camera_fb_t* getImage() {

      FeebeeCam::initializeCamera(1);

      FeebeeCam::_setup->applyToCamera();

      sensor_t *sensor = esp_camera_sensor_get();

      // Set framesize to (very) large      
      sensor->set_framesize(sensor, FRAMESIZE_UXGA);

      // Set highest quality
      //sensor->set_quality(sensor, highQuality);

      flushFrameBuffer();
   
      // Set lights on
      light->turnOn();
      light->flashOn();

      // Capture the actual frame
      camera_fb_t* frameBuffer = esp_camera_fb_get();

      // Turn light off
      light->flashOff();
      light->turnOff();

      FeebeeCam::resetCameraWatchDogTimer();

      return frameBuffer;
      
   }

   
   // Capture a high-res image
   bool uploadImage() {

      if (!FeebeeCam::_setup->_isSetup) {
         cerr << "Missing setup for uploadImage" << endl;
         return false;
      }

      camera_fb_t* image = FeebeeCam::getImage();

      if (image == nullptr)
         return false;

      // Image len is a null terminated string (sic)
      // So need to remove the trailing '\0' null charactger
      const Data data((const Byte*)(image->buf), image->len - 1);

      FeebeeCam::BeeFishStorage storage("/beehive/images/");

      BeeFishId::Id imageId("image/jpeg");

      BString imageURL;
      
      bool sent = storage.setItem(imageId, "image/jpeg" , data);
      imageURL = storage.url();

      esp_camera_fb_return(image);

      if (!sent) {
         FeebeeCam::restartAfterError();
      }
      
      FeebeeCam::settings["lastImageURL"] = imageURL;

      return true;

   }

   double getFrameRate() {
   
      double frameRate = FeebeeCam::framesPerSecond;
      
      FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
      FeebeeCam::frameCount = 0;

      return frameRate;
   }

   void resetCameraWatchDogTimer() {
      cameraWatchDogTimer = millis() + CAMERA_WATCH_DOG_SECONDS;
   }
}