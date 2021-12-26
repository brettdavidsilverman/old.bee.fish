#pragma once

#include "nvs_flash.h"

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
#define SSID_MAX_LEN 36
#define PWD_MAX_LEN 38

typedef struct _feebee_cam_config {
    uint8_t mode;
    char wifi_ssid[SSID_MAX_LEN];
    char wifi_pwd[PWD_MAX_LEN];
} FeebeeCamConfig_t;


void loadFeebeeCamConfig();
void saveFeebeeCamConfig();

bool getWiFiConfig(char *ssid, char *pwd);

bool updateWiFiConfig(const char *ssid, const char *pwd);

char* getWiFiSSID();

char* getWiFiPwd();

#ifdef __cplusplus
}
#endif
