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
#include "status.h"
#include "memory.h"

#define TAG "Camera"


namespace FeebeeCam {

   bool pause = false;
   bool isCameraPaused = false;
   bool stop = false;
   bool isCameraRunning = false;
   float framesPerSecond = 0.0;
   int  frameCount = 0;
   int64_t lastTimeFramesCounted = 0;
   int64_t cameraWatchDogTimer = 0;

   bool isCameraInitialized = false;

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

      if (isCameraInitialized) {            
//         return true;
         
         Serial.println("Deinitializing camera");
         esp_camera_deinit();
                  
      }

      isCameraInitialized = false;

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
         RESTART_AFTER_ERROR("Error initializing camera");
      }

      if (FeebeeCam::_setup)
         FeebeeCam::_setup->applyToCamera();

      FeebeeCam::resetCameraWatchDogTimer();
      
      isCameraInitialized = true;

      Serial.println("Camera Initialized");

      return true;

   }

   bool stopCamera() {

      if (FeebeeCam::isCameraRunning) {

         FeebeeCam::stop = true;
      
         while (FeebeeCam::isCameraRunning)
            delay(1);

      }


      light.turnOff();

      return true;
   }

   bool pauseCamera() {

      if (FeebeeCam::isCameraRunning && !FeebeeCam::isCameraPaused) {

         FeebeeCam::isCameraPaused = false;
         FeebeeCam::pause = true;

         while (!FeebeeCam::isCameraPaused)
            delay(1);

         cerr << "Camera paused" << endl;

      }

      return true;

   }

   bool resumeCamera() {

      if (FeebeeCam::isCameraRunning) {

         FeebeeCam::pause = false;
         
         while (FeebeeCam::isCameraPaused) {
               delay(1);
         }

         light.flashOff();
         light.turnOn();

         cerr << "Camera resumed" << endl;

      }
      else {
         light.turnOff();
      }
      

      return true;

   }

   bool onCamera(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
      
      std::cerr << "Starting Camera Loop..." << std::flush;

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
      client->_chunkedEncoding = true;
      
      if (!client->sendHeaders())
         return false;
      
      bool error = false;

      // Turn on RED
      light.turnOn();

      FeebeeCam::isCameraRunning = true;

      FeebeeCam::_setup->applyToCamera();

      std::cerr << " Ok" << std::endl;

      while(!error && !FeebeeCam::stop) {

         frameBuffer = esp_camera_fb_get();

         if (!frameBuffer) {
            //cerr << "Camera capture failed" << endl;
            continue;
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
         if (frameTime > 0.0) {
            FeebeeCam::framesPerSecond =
               1000.00 * 1000.00 * (float)FeebeeCam::frameCount / (float)frameTime;
         }

         if (FeebeeCam::pause) {

            FeebeeCam::isCameraPaused = true;

            while (FeebeeCam::pause && !FeebeeCam::stop) {
               delay(1);
            }

            FeebeeCam::isCameraPaused = false;

         }

         FeebeeCam::resetCameraWatchDogTimer();

         delay(1);


      }

      FeebeeCam::stop = false;
      FeebeeCam::isCameraPaused = false;
      FeebeeCam::pause = false;
      FeebeeCam::isCameraRunning = false;
      FeebeeCam::framesPerSecond = 0.0;

      light.turnOff();

      if (frameBuffer)
         esp_camera_fb_return(frameBuffer);


      if (!error) {
         if (!client->sendFinalChunk())
            error = true;
      }


      if (error) {
         cerr << "Camera loop ended in error" << endl;
         // This error is normal behaviour,
         // clear the clients error flag
         client->_error = true;
      }
      else
         cerr << "Camera loop ended";
      
      return true;

   }

   bool onCapture(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      // Flush frame buffer, and get the new frame
      camera_fb_t* frameBuffer = FeebeeCam::getImage();
      
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

      FeebeeCam::_setup->applyToCamera();

      return true;
   }
 
   camera_fb_t* getImage() {

      if (!FeebeeCam::isCameraInitialized) {
         FeebeeCam::initializeCamera(1);
      }

      sensor_t *sensor = esp_camera_sensor_get();

      // Set framesize to (very) large      
      sensor->set_framesize(sensor, FRAMESIZE_UXGA);

      // Set highest quality
      //sensor->set_quality(sensor, highQuality);

      flushFrameBuffer();

      // Set lights on
      bool isLightAlreadyOn = light.status();

      light.turnOn();
      light.flashOn();

      // Capture the actual frame
      camera_fb_t* frameBuffer = esp_camera_fb_get();

      light.flashOff();

      if (!isLightAlreadyOn)
         light.turnOff();

      if (frameBuffer)
         FeebeeCam::resetCameraWatchDogTimer();

      return frameBuffer;
      
   }

   
   // Capture a high-res image
   bool uploadImage(BeeFishId::Id& id) {
      
      using namespace std;
      
      if (!FeebeeCam::_setup || !FeebeeCam::_setup->_isSetup) {
         cerr << "Missing setup for uploadImage" << endl;
         return false;
      }

      camera_fb_t* image = FeebeeCam::getImage();
      
      bool sent = false;

      BString imageURL;
         
      if (image) {

         cerr << "Uploading image of size " << image->len << endl;

         const Data data(image->buf, image->len);

         FeebeeCam::BeeFishStorage storage("/beehive/images/");

         sent = storage.setItem(id, "image/jpeg" , data);

         FeebeeCam::status._lastImageURL = storage.url();
         FeebeeCam::status._lastImageTime = FeebeeCam::getDateTime();
         
         esp_camera_fb_return(image);
      }

      if (!sent) {
         RESTART_AFTER_ERROR("Image not uploaded");
      }
      
//      FeebeeCam::_setup->applyToCamera();

      if (FeebeeCam::isTimeInitialized())
         FeebeeCam::status._lastImageTime = FeebeeCam::getDateTime();
      else
         FeebeeCam::status._lastImageTime = "";

      return true;

   }

   double getFrameRate() {
   
      double frameRate = FeebeeCam::framesPerSecond;
      
      FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
      FeebeeCam::frameCount = 0;

      return frameRate;

   }

   void resetCameraWatchDogTimer() {
      cameraWatchDogTimer = millis() + CAMERA_WATCH_DOG_MILLISECONDS;
   }
}