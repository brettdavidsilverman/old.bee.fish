#ifndef FEEBEE_CAM__SETUP_H
#define FEEBEE_CAM__SETUP_H
#include <bee-fish.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "camera.h"
#include "web-client.h"
#include "rtc-bee.h"

namespace FeebeeCam {

    bool initializeSetup();

    bool onSettings(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onRestart(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

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
        bool    _isRTCSetup;
        bool    _isSetup;
    public:
        
        Setup() {
            reset();
        }

        bool initialize() {
            // Initialize NVS
            esp_err_t err = nvs_flash_init();
            if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
                // NVS partition was truncated and needs to be erased
                // Retry nvs_flash_init
                std::cerr << "Erasing mvs flash due to no free pages";
                err = nvs_flash_erase();
                if (err != ESP_OK) {
                    std::cerr << "nvs_flash_erase failed" << std::endl;
                    return false;
                }
                std::cerr << "Initializing nvs flash" << std::endl;
                err = nvs_flash_init();
            }
            
            if (err != ESP_OK) {
                std::cerr << "nvs_flash_init failed" << std::endl;
                return false;
            }

            nvs_handle handle;
            
            err = nvs_open("setup", NVS_READWRITE, &handle);
            
            if (err != ESP_OK) {
                printf("Error (%d) opening NVS handle!\n", err);
                return false;
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
            getValue(handle, "isRTCSetup", _isRTCSetup);
            getValue(handle, "isSetup", _isSetup);

            nvs_close(handle);

            //err = nvs_flash_deinit();
            //ESP_ERROR_CHECK( err );


            std::cerr << "Setup initialized" << std::endl;
            
            return true;

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

            uint8_t u8Value = 0;

            esp_err_t err = nvs_get_u8(handle, key, &u8Value);

            if (err == ESP_OK)
                value = (u8Value == 0) ? false : true;

        }

        void setValue(nvs_handle handle, const char* key, BString value) {
            esp_err_t err = nvs_set_str(handle, key, value.c_str());
            ESP_ERROR_CHECK( err );
        }

        void setValue(nvs_handle handle, const char* key, int value) {
            esp_err_t err = nvs_set_i32(handle, key, value);
            ESP_ERROR_CHECK( err );
        }

        void setValue(nvs_handle handle, const char* key, bool value) {
            esp_err_t err = nvs_set_u8(handle, key, value ? 1 : 0);
            ESP_ERROR_CHECK( err );
        }

        bool save() {
            
            std::cerr << "Saving setup to flash" << std::endl;

            esp_err_t err;

            //esp_err_t err = nvs_flash_init();
            //ESP_ERROR_CHECK( err );
            nvs_handle handle;
            err = nvs_open("setup", NVS_READWRITE, &handle);
            ESP_ERROR_CHECK( err );
            setValue(handle, "label",           _label);
            setValue(handle, "ssid",            _ssid);
            setValue(handle, "password",        _password);
            setValue(handle, "secretHash",      _secretHash);
            setValue(handle, "beehiveVersion",  _beehiveVersion);
            setValue(handle, "host",            _host);

            setValue(handle, "frameSize",       _frameSize);
            setValue(handle, "gainCeiling",     _gainCeiling);
            setValue(handle, "quality",         _quality);
            setValue(handle, "brightness",      _brightness);
            setValue(handle, "contrast",        _contrast);
            setValue(handle, "saturation",      _saturation);
            setValue(handle, "isRTCSetup",      _isRTCSetup);
            setValue(handle, "isSetup",         _isSetup);

            ESP_ERROR_CHECK( err );

            nvs_close(handle);
            
            //err = nvs_flash_deinit();
            //ESP_ERROR_CHECK( err );

            if (err != ESP_OK) {
                std::cerr << "Error saving setup" << std::endl;
                return false;
            }
            else
                std::cerr << "Setup saved" << std::endl;

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
            _isRTCSetup = false; 
            _isSetup = false;
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
                {"isRTCSetup", (bool)FeebeeCam::isRTCSetup()},
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

    extern Setup* _setup;

 
}

#endif