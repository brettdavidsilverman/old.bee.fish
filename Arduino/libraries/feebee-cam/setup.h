#pragma once
#include <bee-fish.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "camera.h"
#include "web-server2.h"

namespace FeebeeCam {


    using namespace BeeFishBString;

    class Setup {
    public:
        BString _label;
        BString _ssid;
        BString _password;
        BString _secretHash;
        BString _beehiveVersion;
        BString _host;
        int     _frameSize;
        int     _gainCeiling;
        int     _quality;
        int     _brightness;
        int     _contrast;
        int     _saturation;
    public:
        
        Setup() {
            
            // Initialize NVS
            esp_err_t err = nvs_flash_init();
            if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
                // NVS partition was truncated and needs to be erased
                // Retry nvs_flash_init
                ESP_ERROR_CHECK(nvs_flash_erase());
                err = nvs_flash_init();
                reset();
            }
            
            ESP_ERROR_CHECK( err );

            nvs_handle handle;
            
            err = nvs_open("setup", NVS_READWRITE, &handle);
            
            if (err != ESP_OK) {
                printf("Error (%d) opening NVS handle!\n", err);
                return;
            }

            getValue(handle, "label", _label);
            getValue(handle, "ssid", _ssid);
            getValue(handle, "password", _password);
            getValue(handle, "secretHash", _secretHash);
            getValue(handle, "beehiveVersion", _beehiveVersion);
            getValue(handle, "host", _host);
            getValue(handle, "frameSize", _frameSize);
            getValue(handle, "gainCeiling", _gainCeiling);
            getValue(handle, "quality", _quality);
            getValue(handle, "brightness", _brightness);
            getValue(handle, "contrast", _contrast);
            getValue(handle, "saturation", _saturation);

            nvs_close(handle);

            nvs_flash_deinit();

            //Serial.println("Setup overriding values for debug");
            //_ssid = "Android";
            //_password = "feebeegeeb3";
        }

        void getValue(nvs_handle handle, const char* key, BString& value) {

            size_t length = 0;

            esp_err_t err = nvs_get_str(handle, key, NULL, &length);

            if (err == ESP_OK) {
                char* buffer = (char*)malloc(length);
                err = nvs_get_str(handle, key, buffer, &length);
                value = BString(buffer);
                delete buffer;
            }
        }

        void getValue(nvs_handle handle, const char* key, int& value) {

            int32_t i32Value = 0;

            esp_err_t err = nvs_get_i32(handle, key, &i32Value);

            if (err == ESP_OK)
                value = i32Value;

        }

        void getValue(nvs_handle handle, const char* key, bool& value) {

            int32_t i32Value = 0;

            esp_err_t err = nvs_get_i32(handle, key, &i32Value);

            if (err == ESP_OK)
                value = (i32Value != 0);

        }

        bool save() {
            esp_err_t err = nvs_flash_init();
            ESP_ERROR_CHECK( err );
            nvs_handle handle;
            err = nvs_open("setup", NVS_READWRITE, &handle);
            ESP_ERROR_CHECK( err );
            nvs_set_str(handle, "label", _label.c_str());
            nvs_set_str(handle, "ssid", _ssid.c_str());
            nvs_set_str(handle, "password", _password.c_str());
            nvs_set_str(handle, "secretHash", _secretHash.c_str());
            nvs_set_str(handle, "beehiveVersion", _beehiveVersion.c_str());

            nvs_set_str(handle, "host", _host.c_str());
            nvs_set_i32(handle, "frameSize", _frameSize);
            nvs_set_i32(handle, "gainCeiling", _gainCeiling);
            nvs_set_i32(handle, "quality", _quality);
            nvs_set_i32(handle, "brightness", _brightness);
            nvs_set_i32(handle, "contrast", _contrast);
            nvs_set_i32(handle, "saturation", _saturation);

            nvs_close(handle);
            nvs_flash_deinit();
            return true;
        }
            
        void reset() {

            // Initial settings
            _host = HOST;
            _frameSize = (double)FRAMESIZE_CIF;
            _gainCeiling = 255;
            _quality = 10;
            _brightness = 0;
            _contrast = 0;
            _saturation = 0;
        }

        BeeFishBScript::Object settings() {

            return BeeFishBScript::Object{
                {"label", _label},
                {"ssid", _ssid},
                {"beehiveVersion", _beehiveVersion},
                {"host", _host},
                {"frameSize", (double)_frameSize},
                {"gainCeiling", (double)_gainCeiling},
                {"quality", (double)_quality},
                {"brightness", (double)_brightness},
                {"contrast", (double)_contrast},
                {"saturation", (double)_saturation}
            };

        }
    
        void applyToCamera() {
            
            Serial.println("Applying camera settings");

            sensor_t *sensor = esp_camera_sensor_get();

            if (sensor) {

                sensor->set_framesize(sensor, (framesize_t)_frameSize);

                sensor->set_gainceiling(sensor, (gainceiling_t)_gainCeiling);

                sensor->set_quality(sensor, (int)_quality);

                sensor->set_brightness(sensor, (int)_brightness);

                sensor->set_contrast(sensor, (int)_contrast);

                sensor->set_saturation(sensor, (int)_saturation);

                // Turn the camera the right way round
                sensor->set_vflip(sensor, (int)1);
                sensor->set_hmirror(sensor, (int)1);

                std::cerr << "Setup applied to camera" << std::endl;
            }
            else
                std::cerr << "Error applying setup to camera" << std::endl;

        }

    };

    extern Setup setup;

    bool onSettings(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onRestart(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

}