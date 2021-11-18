#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

#include "timer_cam_config.h"
#include "test.h"
#include "battery.h"
//#include "bm8563.h"
#include "led.h"
#include "network.h"
#include "protocol.h"
#include "cam_cmd.h"
#include "app_httpd.h"
#include "i2c.h"
#include "light.h"
#include "error.h"

#define TAG "TIMERCAM"

static camera_config_t camera_config = {
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

    .pixel_format = PIXFORMAT_JPEG,//YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_FHD, //FRAMESIZE_UXGA, //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 10, //0-63 lower number means higher quality
    .fb_count = 2  //if more than one, i2s runs in continuous mode. Use only with JPEG
};


char CAM_LOGO[] =
" _____ _                      ____                \r\n"
"|_   _(_)_ __ ___   ___ _ __ / ___|__ _ _ __ ___  \r\n"
"  | | | | '_ ` _ \\ / _ \\ '__| |   / _` | '_ ` _ \\ \r\n"
"  | | | | | | | | |  __/ |  | |__| (_| | | | | | |\r\n"
"  |_| |_|_| |_| |_|\\___|_|   \\____\\__,_|_| |_| |_|\r\n";


bool restart = false;
volatile bool init_finish = false;

//mcp23008_t mcp23008;

extern "C" void app_main()
{
    esp_err_t ret = ESP_OK;

    led_init(CAMERA_LED_GPIO);
    led_brightness(256);
    
    //Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    light = new Light;
    light->turnOn();

    if (beeFishTest() != ESP_OK) {
        printf("Bee Fish Test Failed\n");
        while (1)
            ;
    }

    //bm8563_init();
    //bat_init();
    //bat_hold_output();
    
 //   esp_log_level_set(TAG, ESP_LOG_ERROR);
 //   printf("%s", CAM_LOGO);

    ret = esp_camera_init(&camera_config);
    CHECK_ERROR(ret, TAG, "Error initializing camera");

    InitTimerCamConfig();
    InitCamFun();

    esp_task_wdt_init(1, false);
    esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(0));
    
    init_finish = true;

    start_webserver("Bee", "feebeegeeb3");

    light->turnOff();
}

