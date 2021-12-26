#include "feebee-cam-config.h"
#include <bee-fish.h>
#include "esp_camera.h"

#define CAM_NAMESPACE "feebee-cam"
#define CAM_CONFIG_KEY "cam-config"

static FeebeeCamConfig_t feebee_cam_config;


void loadFeebeeCamConfig() {
    nvs_handle_t handle;
    esp_err_t err;
    err = nvs_open(CAM_NAMESPACE, NVS_READWRITE, &handle);
    size_t length;
    err = nvs_get_blob(handle, CAM_CONFIG_KEY, &feebee_cam_config, &length);
    // nvs not found device config
    if (err != ESP_OK) { 
        feebee_cam_config.wifi_pwd[0] = '\0';
        feebee_cam_config.wifi_ssid[0] = '\0';
        nvs_set_blob(handle, CAM_CONFIG_KEY, &feebee_cam_config, sizeof(FeebeeCamConfig_t));
        nvs_commit(handle);
    }
    nvs_close(handle);
    
    err = esp_camera_load_from_nvs(CAM_NAMESPACE);

    if (err != ESP_OK) {
        sensor_t *s = esp_camera_sensor_get();
        s->set_framesize(s, FRAMESIZE_CIF);
        s->set_quality(s, 5);
        s->set_vflip(s, 1); //flip it back
        s->set_hmirror(s, 1);
        s->set_gainceiling(s, GAINCEILING_16X);
    }
}

void saveFeebeeCamConfig() {
    nvs_handle_t handle;
    nvs_open(CAM_NAMESPACE, NVS_READWRITE, &handle);
    nvs_set_blob(handle, CAM_CONFIG_KEY, &feebee_cam_config, sizeof(FeebeeCamConfig_t));
    nvs_commit(handle);
    nvs_close(handle);
    esp_camera_save_to_nvs(CAM_NAMESPACE);
}


bool getWiFiConfig(char *ssid, char *pwd) {
    if (strlen(feebee_cam_config.wifi_ssid) == 0) {
        return false;
    }

    memcpy(ssid, feebee_cam_config.wifi_ssid, strlen(feebee_cam_config.wifi_ssid) + 1);
    memcpy(pwd, feebee_cam_config.wifi_pwd, strlen(feebee_cam_config.wifi_pwd) + 1);
    return true;
}

bool updateWiFiConfig(const char *ssid, const char *pwd) {
    int ssid_len = strlen(ssid);
    int pwd_len = strlen(pwd);

    if (ssid_len >= SSID_MAX_LEN || pwd_len >= PWD_MAX_LEN) {
        return false;
    }
    
    feebee_cam_config.wifi_ssid[ssid_len] = '\0';
    feebee_cam_config.wifi_pwd[pwd_len] = '\0';

    memcpy(feebee_cam_config.wifi_ssid, ssid, ssid_len);
    memcpy(feebee_cam_config.wifi_pwd, pwd, pwd_len);

    return true;
}

char* getWiFiSSID() {
    return feebee_cam_config.wifi_ssid;
}

char* getWiFiPwd() {
    return feebee_cam_config.wifi_pwd;
}