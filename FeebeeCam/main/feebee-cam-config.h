#ifndef FEEBEECAM_CONFIG
#define FEEBEECAM_CONFIG

#include "nvs_flash.h"
#include "esp_camera.h"
#include "error.h"
#include <bee-fish.h>

#define TAG "FeebeeCam Config"

/* Define ------------------------------------------------------------------- */
// pin config
#define CAM_PIN_PWDN    -1 //power down is not used
#define CAM_PIN_RESET   15 //software reset will be performed
#define CAM_PIN_XCLK    27
#define CAM_PIN_VSYNC   22
#define CAM_PIN_PCLK    21
#define CAM_PIN_HREF    26

#define CAM_PIN_SIOC    23
#define CAM_PIN_SIOD    25

#define CAM_PIN_D7      19
#define CAM_PIN_D6      36
#define CAM_PIN_D5      18
#define CAM_PIN_D4      39
#define CAM_PIN_D3      5
#define CAM_PIN_D2      34
#define CAM_PIN_D1      35
#define CAM_PIN_D0      32

#define CAM_XCLK_FREQ   20000000

#define CAMERA_LED_GPIO 2

#define Ext_PIN_1 4
#define Ext_PIN_2 13

#define BAT_HOLD_PIN 33
#define BAT_ADC_PIN 33

#ifdef __cplusplus
extern "C" {
#endif

// mod config
#define SSID_MAX_LENGTH 36
#define PASSWORD_MAX_LENGTH 38
#define SECRET_HASH_LENGTH 128

#define CAM_NAMESPACE "feebeeCam"
#define CAM_CONFIG_KEY "cam-config"

struct FeebeeCamConfig {
    bool setup;
protected:
    char _ssid[SSID_MAX_LENGTH];
    char _password[PASSWORD_MAX_LENGTH];
    char _secretHash[SECRET_HASH_LENGTH + 1];
    
public:
    FeebeeCamConfig() {
        this->clear();
    }

    BString getSSID() {
        return _ssid;
    }

    BString getPassword() {
        return _password;
    }

    BString getSecretHash() {
        return _secretHash;
    }

    bool update(BString ssid, BString password, BString secretHash) {

        cerr << "Updating feebeeCamConfig" << endl;

        FeebeeCamConfig::clear();

        std::string _ssid = ssid;
        std::string _password = password;
        std::string _secretHash = secretHash;

        if ( _ssid.size() >= SSID_MAX_LENGTH || 
            _password.size() >= PASSWORD_MAX_LENGTH ||
            _secretHash.size() != SECRET_HASH_LENGTH )
        {
            return false;
        }
        
        Serial.println("Copying values to feebeeCamConfig");

        memcpy(FeebeeCamConfig::_ssid, _ssid.c_str(), _ssid.size());
        memcpy(FeebeeCamConfig::_password, _password.c_str(), _password.size());
        memcpy(FeebeeCamConfig::_secretHash, _secretHash.c_str(), _secretHash.size());
        
        this->setup = true;

        save();

        return true;
    }

    bool save() {
        Serial.println("Saving feebeeCamConfig to " CAM_NAMESPACE);

        nvs_handle_t handle;
        esp_err_t res = nvs_open(CAM_NAMESPACE, NVS_READWRITE, &handle);
        CHECK_ERROR(res, TAG, "Open nvs for read/write");

        res = nvs_set_blob(handle, CAM_CONFIG_KEY, this, sizeof(FeebeeCamConfig));
        CHECK_ERROR(res, TAG, "set blob nvs");

        res = nvs_commit(handle);
        CHECK_ERROR(res, TAG, "Commit nvs");

        nvs_close(handle);
        res =  esp_camera_save_to_nvs(CAM_NAMESPACE);
        CHECK_ERROR(res, TAG, "esp_camera_save_to_nvs");
        Serial.println("FeebeeCamConfig saved successfully");

        return true;
    }

    bool load() {
        Serial.println("Loading from nvs");

        nvs_handle_t handle;
        esp_err_t err;
        err = nvs_open(CAM_NAMESPACE, NVS_READWRITE, &handle);
        CHECK_ERROR(err, TAG, "Opening nvs");

        size_t length = sizeof(FeebeeCamConfig);
        err = nvs_get_blob(handle, CAM_CONFIG_KEY, this, &length);
                    
        if (err == ESP_OK) {
            Serial.print("Loaded FeebeeCam settings from nvs: ");
        }
        else { 
            Serial.println("feebeeCamConfig not found: " CAM_CONFIG_KEY);
        }
        nvs_close(handle);
        
        // Loading camera settings form nvs
        Serial.println("Loading camera settings from " CAM_NAMESPACE);
        err = esp_camera_load_from_nvs(CAM_NAMESPACE);

        if (err != ESP_OK) {
            Serial.println("Camera settings not found");
            Serial.println("Initializing camera sensor");
            sensor_t *s = esp_camera_sensor_get();
            s->set_framesize(s, FRAMESIZE_CIF);
            s->set_quality(s, 10);
            s->set_vflip(s, 1); //flip it back
            s->set_hmirror(s, 1);
            s->set_gainceiling(s, GAINCEILING_16X);
        }
        return true;
    }

    void clear() {
        cerr << "memset  feebeeCamConfig" << endl;
        memset(this, 0, sizeof(FeebeeCamConfig));
    }

};

extern FeebeeCamConfig feebeeCamConfig;

#ifdef __cplusplus
}
#endif

#endif