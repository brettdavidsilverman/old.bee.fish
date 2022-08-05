#include <Arduino.h>
#include <queue>
#include <mutex>
#include "camera.h"
#include "light.h"
#include "setup.h"
#include "commands.h"
#include "weather.h"
#include "commands.h"

#define TAG "Camera"


namespace FeebeeCam {

    volatile bool pause = false;
    volatile bool isPaused = false;
    volatile bool stop = false;
    volatile bool isCameraRunning = false;
    volatile unsigned long lastTimeCameraUsed;
    volatile float framesPerSecond = 0.0;
    volatile int  frameCount = 0;
    volatile int64_t lastTimeFramesCounted = 0;

    //const uint8_t highQuality = 5;

    bool cameraInitialized = false;

    #define PART_BOUNDARY "123456789000000000000987654321"
    #define _STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
    #define _STREAM_BOUNDARY "\r\n--" PART_BOUNDARY "\r\n"
    #define _STREAM_PART  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"

    FrameBufferQueue frameBufferQueue(FRAME_BUFFER_COUNT);
    

    bool initializeCamera(int frameBufferCount)
    {
        Serial.println("Initializing camera");
        
        if (cameraInitialized)
            esp_camera_deinit();

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
            .frame_size = FRAMESIZE_QXGA, 

            .jpeg_quality = 0, //0-63 lower number means higher quality
            .fb_count = frameBufferCount   //if more than one, i2s runs in continuous mode. Use only with JPEG
        };

        esp_err_t ret = esp_camera_init(&camera_config);

        if (ret != ESP_OK) {
            Serial.println("Error initializing camera");
        }

        setup.applyToCamera();

        cameraInitialized = true;

        Serial.println("Camera Initialized");

        return true;

    }

    bool onCamera(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        
        Serial.println("Camera");

        if (FeebeeCam::isCameraRunning) {
            FeebeeCam::stop = true;
            while  (FeebeeCam::isCameraRunning)
                delay(1);
        }
        FeebeeCam::isCameraRunning = true;
        FeebeeCam::stop = false;
        
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

        FeebeeCam::setup.applyToCamera();

        bool error = false;

        if (!frameBufferQueue.start())
            return false;

        // Turn on RED
        FeebeeCam::light->turnOn();

        delay(10);

        while(!error && !FeebeeCam::stop) {

            FeebeeCam::lastTimeCameraUsed = millis();

//            frameBuffer = esp_camera_fb_get();
            frameBuffer = frameBufferQueue.pop_front();

            if (!frameBuffer) {
                cerr << "Camera capture failed" << endl;
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
            FeebeeCam::framesPerSecond =
                1000.00 * 1000.00 * (float)FeebeeCam::frameCount / (float)frameTime;

            if (FeebeeCam::pause) {

                Serial.println("Paused");
                FeebeeCam::isPaused = true;

                while (FeebeeCam::pause) {
                    delay(10);
                }

            }

            if (FeebeeCam::isPaused) {

                Serial.println("Resuming");
                FeebeeCam::isPaused = false;

                // Turn on RED
                FeebeeCam::light->turnOn();            

            }

            delay(1);
        }

        FeebeeCam::light->turnOff();

        if (frameBuffer)
            esp_camera_fb_return(frameBuffer);

        frameBufferQueue.stop();
        std::cerr << "Queue stopped" << std::endl;

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
        
        uint32_t _runningColor = 0;

        if (FeebeeCam::isCameraRunning) {

            FeebeeCam::isPaused = false;
            FeebeeCam::pause = true;

            while (!FeebeeCam::isPaused)
                delay(10);
        }

        // Set capture specific settings...

        sensor_t *sensor = esp_camera_sensor_get();

        // Largest frame size?
/*
    FRAMESIZE_96X96,    // 96x96
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_HQVGA,    // 240x176
    FRAMESIZE_240X240,  // 240x240
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_CIF,      // 400x296
    FRAMESIZE_HVGA,     // 480x320
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_HD,       // 1280x720
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
    // 3MP Sensors
    FRAMESIZE_FHD,      // 1920x1080
    FRAMESIZE_P_HD,     //  720x1280
    FRAMESIZE_P_3MP,    //  864x1536
    FRAMESIZE_QXGA,     // 2048x1536
    // 5MP Sensors
    FRAMESIZE_QHD,      // 2560x1440
    FRAMESIZE_WQXGA,    // 2560x1600
    FRAMESIZE_P_FHD,    // 1080x1920
    FRAMESIZE_QSXGA,    // 2560x1920
    FRAMESIZE_INVALID
*/
        //sensor->set_framesize(sensor, FRAMESIZE_HD);

        // Set framesize to (very) large        
        sensor->set_framesize(sensor, FRAMESIZE_UXGA);

        // Set highest quality
        //sensor->set_quality(sensor, highQuality);
        

        // Set lights on
        light->flashOn();
        light->turnOn();

        // Flush frame buffer, and get the new frame
        camera_fb_t* frameBuffer = frameBufferQueue.flush();

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
                "\r\n" <<
                image;

            esp_camera_fb_return(frameBuffer);

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
        FeebeeCam::setup.applyToCamera();

        frameBuffer = frameBufferQueue.flush(); 
        if (frameBuffer)
            esp_camera_fb_return(frameBuffer);

        FeebeeCam::pause = false;

        return true;
    }
 
    bool onCommand(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishBScript::Object object;
        object["status"] = BeeFishBScript::Null();
        object["message"] = "Invalid command";

        BeeFishBScript::ObjectPointer request = 
            (BeeFishBScript::ObjectPointer)(client->_parser.value());

        // Command
        BString command = (*request)["command"];

        bool _putToSleep = false;

        if (command == "stop") {
            FeebeeCam::stop = true;
            object["status"] = true;
            object["message"] = "Camera stopped";
        }
        else if (command == "reset") {
            setup.reset();
            setup.save();
            setup.applyToCamera();
            object["status"] = true;
            object["message"] = "Camera reset";
        }
        else if (command == "sleep") {
            object["status"] = true;
            object["message"] = "Camera put to sleep";
            object["redirectURL"] = HOST "/beehive/";
            _putToSleep = true;
        }
                
        
        Serial.print("Sent Camera command ");
        Serial.println(command.c_str());

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        client->_contentType = "text/javascript";
        client->sendHeaders();

        stream << object;

        client->sendFinalChunk();

        if (_putToSleep) {
            FeebeeCam::commands.push(FeebeeCam::PUT_TO_SLEEP);
        }

        return true;

    }


    BeeFishBString::Data* getImage() {

        FeebeeCam::setup.applyToCamera();

        sensor_t *sensor = esp_camera_sensor_get();

        // Set framesize to (very) large        
        sensor->set_framesize(sensor, FRAMESIZE_UXGA);

        // Set highest quality
        //sensor->set_quality(sensor, highQuality);
    
        // Set lights on
        light->flashOn();
        light->turnOn();

        delay(100);

        // Flush one frame
        camera_fb_t* frameBuffer = esp_camera_fb_get();
        if (frameBuffer)
            esp_camera_fb_return(frameBuffer);

        // Capture the actual frame
        frameBuffer = esp_camera_fb_get();

        // Turn light off
        light->flashOff();
        light->turnOff();


        if (frameBuffer) {
            BeeFishBString::Data* image = new Data(frameBuffer->buf, frameBuffer->len, true);
            esp_camera_fb_return(frameBuffer);
            return image;
        }
        else {
            cerr << "Couldn't capture frame" << endl;
        }

        return nullptr;
       
    }

    
    double getFrameRate() {
    
        double frameRate = FeebeeCam::framesPerSecond;
        
        FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
        FeebeeCam::frameCount = 0;

        return frameRate;
    }


}