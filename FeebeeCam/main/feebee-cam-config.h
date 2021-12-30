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

#define CAM_NAMESPACE "feebee-cam"
#define CAM_CONFIG_KEY "cam-config"

typedef struct _feebeeCamConfig {
    bool setup;
    char wifiSSID[SSID_MAX_LENGTH];
    char wifiPassword[PASSWORD_MAX_LENGTH];
    char secretHash[SECRET_HASH_LENGTH + 1];
    
    BString getWiFiSSID() {
        return wifiSSID;
    }

    BString getWiFiPassword() {
        return wifiPassword;
    }

    BString getSecretHash() {
        return secretHash;
    }

    bool update(BString ssid, BString password, BString secretHash) {

        cerr << "Checking sizes" << endl;

        if ( ssid.size() >= SSID_MAX_LENGTH || 
            password.size() >= PASSWORD_MAX_LENGTH ||
            secretHash.size() != SECRET_HASH_LENGTH )
        {
            return false;
        }
        
        cerr << "memset  feebeeCamConfig" << endl;

        memset(this, '\0', sizeof(*this));

        cerr << "Copying values to feebeeCamConfig" << endl;

        std::string strSSID = ssid.str();
        std::string strPassword = password.str();

        memcpy(this->wifiSSID, strSSID.c_str(), strSSID.size());
        memcpy(this->wifiPassword, strPassword.c_str(), strPassword.size());
        memcpy(this->secretHash, secretHash.str().c_str(), secretHash.size());
        
        this->setup = true;
        
        save();

        return true;
    }

    bool save() {
        nvs_handle_t handle;
        nvs_open(CAM_NAMESPACE, NVS_READWRITE, &handle);
        esp_err_t res = nvs_set_blob(handle, CAM_CONFIG_KEY, this, sizeof(*this));
        if (res == ESP_OK)
            res = nvs_commit(handle);

        if (res == ESP_OK)
            nvs_close(handle);

        if (res == ESP_OK)
            res =  esp_camera_save_to_nvs(CAM_NAMESPACE);

        CHECK_ERROR(res, TAG, "Saving feebeeCamConfig to nvs");

        Serial.println("FeebeeCamConfig saved successfully");

        return true;
    }

    void load() {
        Serial.println("Loading from nvs");

        nvs_handle_t handle;
        esp_err_t err;
        err = nvs_open(CAM_NAMESPACE, NVS_READWRITE, &handle);
        CHECK_ERROR(err, TAG, "Opening nvs");

        size_t length;
        err = nvs_get_blob(handle, CAM_CONFIG_KEY, this, &length);
        
        // nvs not found device config
        if (err == ESP_OK) {
            Serial.println("Loaded wifi settings from nvs");
            Serial.print("Secret: ");
            Serial.println(this->getSecretHash());
        }
        else { 
            Serial.println("feebeeCamConfig not found: " CAM_CONFIG_KEY);
            memset(this, 0, sizeof(*this));
            err = nvs_set_blob(handle, CAM_CONFIG_KEY, this, sizeof(*this));
            CHECK_ERROR(err, TAG, "Setting memset blob");
            err = nvs_commit(handle);
            CHECK_ERROR(err, TAG, "Committing nvs");
        }
        nvs_close(handle);
        
        err = esp_camera_load_from_nvs(CAM_NAMESPACE);
        CHECK_ERROR(err, TAG, "Loading camera config from nvs");

        if (err != ESP_OK) {
            Serial.println("Initializing camera sensor");
            sensor_t *s = esp_camera_sensor_get();
            s->set_framesize(s, FRAMESIZE_CIF);
            s->set_quality(s, 10);
            s->set_vflip(s, 1); //flip it back
            s->set_hmirror(s, 1);
            s->set_gainceiling(s, GAINCEILING_16X);
        }
    }



} FeebeeCamConfig_t;

extern FeebeeCamConfig_t feebeeCamConfig;

#ifdef __cplusplus
}
#endif

#endif