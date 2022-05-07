#include <Arduino.h>
#include "camera.h"
#include "light.h"
#include "settings.h"

#define TAG "Camera"

namespace FeebeeCam {

    volatile bool pause = false;
    volatile bool isPaused = false;
    volatile bool stop = false;
    volatile bool isRunning = false;

    volatile float framesPerSecond = 0.0;
    volatile int  frameCount = 0;
    volatile int64_t lastTimeFramesCounted = 0;

    bool cameraInitialized = false;

    #define PART_BOUNDARY "123456789000000000000987654321"
    #define _STREAM_CONTENT_TYPE "multipart/x-mixed-replace;boundary=" PART_BOUNDARY
    #define _STREAM_BOUNDARY "\r\n--" PART_BOUNDARY "\r\n"
    #define _STREAM_PART  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n"

    void flushFrameBuffer();

    void initializeCamera()
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
            .fb_count = FRAME_BUFFER_COUNT   //if more than one, i2s runs in continuous mode. Use only with JPEG
        };

        esp_err_t ret = esp_camera_init(&camera_config);

        if (ret != ESP_OK) {
            Serial.println("Error initializing camera");
        }

        settings.applySettings();

        cameraInitialized = true;

        Serial.println("Camera Initialized");
    }

    bool onCameraGet(BeeFishWeb::WebRequest& request, WiFiClient& client) {
        
        Serial.println("Camera get");
        
        camera_fb_t * frameBuffer = NULL;
        esp_err_t res = ESP_OK;
        size_t _jpg_buf_len;
        uint8_t * _jpg_buf;
        char * part_buf[64];
        
        Data streamBoundary((byte*)_STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));

        client.println("HTTP/1.1 200 OK");
        client.println("Connection: keep-alive");
        client.println("Content-Type: " _STREAM_CONTENT_TYPE);
        client.println("Access-Control-Allow-Origin: null");
        client.println("Cache-Control: no-store, max-age=0");
        client.println("Transfer-Encoding: chunked");
        client.println();
        
        
        FeebeeCam::stop = false;
        FeebeeCam::isRunning = true;

        Serial.println("Starting camera loop");


        // Turn on RED
        light.turnOn();
        
        while(client && !FeebeeCam::stop){
            
            //esp_task_wdt_restart();
            //taskYIELD();
            //yield();

            frameBuffer = esp_camera_fb_get();
            if (!frameBuffer) {
                DEBUG_OUT("Camera capture failed");
                break ;
            } 

            const Data capturedFrame(frameBuffer->buf, frameBuffer->len);

            size_t headerLength = snprintf((char *)part_buf, 64, _STREAM_PART, capturedFrame.size());

            WiFiWebServer::sendChunk(client, Data((byte*)part_buf, headerLength));
            WiFiWebServer::sendChunk(client, capturedFrame);
            WiFiWebServer::sendChunk(client, streamBoundary);

            esp_camera_fb_return(frameBuffer);

            ++frameCount;

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
                light.turnOn();            

            }

        }

        WiFiWebServer::sendChunk(client);
        
        Serial.println("Camera loop ended");

        light.turnOff();

        FeebeeCam::stop = false;
        FeebeeCam::isRunning = false;
        FeebeeCam::isPaused = false;
        FeebeeCam::pause = false;

        return true;
    }

    bool onCaptureGet(BeeFishWeb::WebRequest& request, WiFiClient& client) {

        // Set pause flag to initiate stop camera stream procecss
        
        uint32_t _runningColor = 0;

        if (FeebeeCam::isRunning) {

            FeebeeCam::isPaused = false;
            FeebeeCam::pause = true;

            while (!FeebeeCam::isPaused)
                delay(10);
        }

        // Set capture specific settings...
        sensor_t *sensor = esp_camera_sensor_get();

        // Highest quality?
        sensor->set_quality(sensor, 5);

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
        sensor->set_framesize(sensor, FRAMESIZE_HD);

        // Flush the frame buffer queue
        flushFrameBuffer();
        
        // Set lights on
        light.flashOn();
        
        // Take the picture
        camera_fb_t* frameBuffer = esp_camera_fb_get();

        // Turn light off
        light.turnOff();

        if (!frameBuffer) {
            Serial.println("Camera capture failed");
            client.println("HTTP/1.1 500 Error");
            client.println("Connection: keep-alive");
            client.println("Access-Control-Allow-Origin: null");
            client.println("Cache-Control: no-store, max-age=0");
            client.println("Content-Type: text/plain");
            client.println();
            client.println("{\"error\": \"Error with camera capturing frame\"}");
        } 
        else {

            const BeeFishBString::Data data(frameBuffer->buf, frameBuffer->len);

            client.println("HTTP/1.1 200 OK");
            client.println("Connection: keep-alive");
            client.println("Access-Control-Allow-Origin: null");
            client.println("Cache-Control: no-store, max-age=0");
            client.println("Content-Type: image/jpeg");
            client.print("Content-Length: ");
            client.println(data.size());
            client.println();

            client.write(data.data(), data.size());

            esp_camera_fb_return(frameBuffer);

        }
        

        // Should use settings here
        sensor->set_framesize(sensor, FRAMESIZE_CIF);//FRAMESIZE_XGA); //FRAMESIZE_QXGA
        sensor->set_quality(sensor, 10);

//        flushFrameBuffer();

        FeebeeCam::pause = false;

        return true;
    }
 
    bool onCommandPost(BeeFishWeb::WebRequest& request, WiFiClient& client) {
        
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishBScript::Object object;
        object["status"] = BeeFishBScript::Null();
        object["message"] = "Invalid command";
        
        // Command
        BString command;
        JSONParser::OnValue oncommand = 
            [&object, &command, &client](const BString& key, JSON& json) {
                command = json.value();
                camera_fb_t* fb = nullptr;

                if (command == "stop") {
                    FeebeeCam::stop = true;
                    object["status"] = true;
                    object["message"] = "Camera stopped";
                }

                WiFiWebServer::sendResponse(client, object);

                if (fb)
                    esp_camera_fb_return(fb);

            };
        
        BeeFishJSON::JSON json;

        BeeFishJSON::JSONParser parser(json);

        parser.invokeValue("command", oncommand);

        WiFiWebServer::parseRequest(parser, client);
        
        Serial.print("Sent Camera command ");
        Serial.println(command.c_str());

        return true;

    }

    bool onSettings(BeeFishWeb::WebRequest& request, WiFiClient& client) {
        
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishBScript::Object output;
        sensor_t *sensor = esp_camera_sensor_get();

        BString message;

        if (request.method() == "POST") {

            output["status"] = BeeFishBScript::Null();
            output["message"] = "Invalid setting";
            
            // Command
            BeeFishJSON::Object::OnKeyValue onsetting = 
                
                [&output, &client, &message, &sensor](const BString& key, JSON& json) {
                    
                    Serial.print("On Setting: ");
                    Serial.println(key.c_str());

                    const BeeFishBString::BString& stringValue = json.value();
                    int value = atoi(stringValue.c_str());
                    const BString& setting = key;
                    
                    if (setting == "frameSize") {
                        sensor->set_framesize(sensor, (framesize_t)value);
                        message = "Frame size set to " + stringValue;
                    }
                    /*
                    else if (setting == "gainCeiling") {
                        sensor->set_gainceiling(sensor, (gainceiling_t)value);
                        message = "Gain ceiling set to " + stringValue;
                    }
                    else if (setting == "quality") {
                        sensor->set_quality(sensor, value);
                        message = "Quality set to " + stringValue;
                    }
                    else if (setting == "brightness") {
                        sensor->set_brightness(sensor, value);
                        message = "Brightness  set to " + stringValue;
                    }
                    else if (setting == "contrast") {
                        sensor->set_contrast(sensor, value);
                        message = "Contrast  set to " + stringValue;
                    }
                    else if (setting == "saturation") {
                        sensor->set_saturation(sensor, value);
                        message = "Saturation  set to " + stringValue;
                    }
                    */
                };
            
            BeeFishJSON::Object json;

            BeeFishJSON::JSONParser parser(json);
            json.setOnKeyValue(onsetting);

            WiFiWebServer::parseRequest(parser, client);

        }
        else {
            // GET
            message = "Retrieved camera settings";
        }


        output = settings;
        /*
        output["frameSize"]     = settings["frameSize"];
        output["gainCeiling"]   = (int)(sensor->status.gainceiling);
        output["quality"]       = sensor->status.quality;
        output["brightness"]    = sensor->status.brightness;
        output["contrast"]      = sensor->status.contrast;
        output["saturation"]    = sensor->status.saturation;
        */
        output["message"] = message;
        
        WiFiWebServer::sendResponse(client, output);

        Serial.println(message.c_str());
        
        return true;
    }


    // Flush the frame buffer queue
    void flushFrameBuffer() {
        for (int i = 0; i < FRAME_BUFFER_COUNT; ++i) {
            camera_fb_t* frameBuffer = esp_camera_fb_get();
            if (frameBuffer)
                esp_camera_fb_return(frameBuffer);
        }
    }

    double getFramerate() {
    
        double framerate = FeebeeCam::framesPerSecond;
        
        FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
        FeebeeCam::frameCount = 0;

        return framerate;
    }

 
}