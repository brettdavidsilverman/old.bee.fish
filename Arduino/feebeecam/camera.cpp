#include <Arduino.h>
#include "esp_camera.h"
#include "camera-pins.h"
#include "camera.h"

#define TAG "Camera"

namespace FeebeeCam {

    bool stop = false;
    bool isRunning = true;
    float framesPerSecond = 0.0;
    int  frameCount = 0;
    int64_t lastTimeFramesCounted = 0;

    #define PART_BOUNDARY "123456789000000000000987654321"
    static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
    static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
    static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

    void onCameraGet(httpsserver::HTTPRequest * request, httpsserver::HTTPResponse * response) {
        
        Serial.println("Camera get");
        
        
        camera_fb_t * fb = NULL;
        esp_err_t res = ESP_OK;
        size_t _jpg_buf_len;
        uint8_t * _jpg_buf;
        char * part_buf[64];

        response->setHeader("Content-Type", _STREAM_CONTENT_TYPE);
        response->setHeader("Access-Control-Allow-Origin", "*");
        response->setHeader("Cache-Control", "no-store, max-age=0");
        response->setHeader("Transfer-Encoding", "chunked");

        // light->turnOn();
        
        FeebeeCam::stop = false;
        FeebeeCam::isRunning = true;

        while(!FeebeeCam::stop){
            
            //esp_task_wdt_restart();
            //taskYIELD();
            //yield();

            fb = esp_camera_fb_get();
            if (!fb) {
                DEBUG_OUT("Camera capture failed");
                break ;
            } 

            _jpg_buf_len = fb->len;
            _jpg_buf = fb->buf;

            if(res == ESP_OK){
                size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);

                sendChunk(response, (byte*)part_buf, hlen);
                sendChunk(response, _jpg_buf, _jpg_buf_len);
                sendChunk(response, (byte*)_STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
            }

            esp_camera_fb_return(fb);

            if(res != ESP_OK){
                break;
            }

            ++frameCount;

            int64_t frameEndTime = esp_timer_get_time();
            int64_t frameTime = frameEndTime - FeebeeCam::lastTimeFramesCounted;
            framesPerSecond =
                1000.00 * 1000.00 * (float)FeebeeCam::frameCount / (float)frameTime;

        }

        sendChunk(response, nullptr, 0);
        
        stop = false;
        isRunning = false;

        //light->turnOff();
        
    }

    void initializeCamera()
    {
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
            .frame_size = FRAMESIZE_UXGA,    // FRAMESIZE_P_3MP,   ////FRAMESIZE_UXGA, //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

            .jpeg_quality = 2, //0-63 lower number means higher quality
            .fb_count = FRAME_BUFFER_COUNT         //if more than one, i2s runs in continuous mode. Use only with JPEG
        };

        esp_err_t ret = esp_camera_init(&camera_config);

        if (ret != ESP_OK)
            Serial.println("Error initializing camera");

        sensor_t *s = esp_camera_sensor_get();

        if (s) {
            Serial.println("Initializing camera sensor");
            s->set_framesize(s, FRAMESIZE_CIF);
            s->set_quality(s, 10);
            s->set_vflip(s, 1); //flip it back
            s->set_hmirror(s, 1);
            s->set_gainceiling(s, GAINCEILING_16X);
        }

        esp_camera_load_from_nvs("user");
        
        Serial.println("Camera Initialized");
    }

}