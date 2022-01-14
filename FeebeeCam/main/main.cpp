#include <stdio.h>

#include "config.h"
#include <Arduino.h>
#include <WiFi.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

#include "feebee-cam-config.h"
#include "test.h"
#include "battery.h"

#include "bme280.h"
#include <bee-fish.h>

//#include "bm8563.h"
#include "led.h"
#include "network.h"
#include "app_httpd.h"
#include "i2c.h"
#include "light.h"
#include "web-request.h"

#define TAG "FEEBEECAM"

void testWebRequest();
void initializeBeeFish();
void initializeBattery();
void initializeLight();
void initializeLED();
void initializeWeather();
void FeebeeCam::initializeCamera();
void initializeStorage();
#ifdef SECURE_SOCKETS
void initializeSSLCert();
#endif
void FeebeeCam::printStatus();
void registerBeehiveLink();

bool restart = false;

using namespace FeebeeCam;

//FeebeeCam::WebRequest webRequest("/", "");


//mcp23008_t mcp23008;

void setup()
{

   Serial.begin(115200);

   while (!Serial)
      ;

   Serial.print("Starting up....");


   initializeBeeFish();
   initializeBattery();
   FeebeeCam::initializeCamera();
   
   initializeStorage();

   initializeWeather();
   initializeLED();
#ifdef SECURE_SOCKETS
   initializeSSLCert();
#endif

   initializeWiFi();
   startWebServers();

   initializeLight();
   testWebRequest();
   FeebeeCam::printStatus();

   //bm8563_init();

   INFO(TAG, "FeebeeCam Initialized");
}

unsigned long lastConnectedTime = 0;
unsigned long lastCheckTime = 0;

void loop()
{
   // Check every 1 second
   if (millis() - lastCheckTime > 1000) 
   {

      lastCheckTime = millis();

      // Check for any connected device
      if ( ( WiFi.softAPgetStationNum() > 0 ) || 
           ( WiFi.isConnected() ) ) 
      {
         // Set last connected time
         lastConnectedTime = millis();
      }
      else if ( ( millis() - lastConnectedTime ) > 30000 )
      {
         // No connection in 30sec, restart
         Serial.println("Restarting...");
         ESP.restart();
      }

      // If Connected, and have an IP Address, register beehive link
      if (FeebeeCam::registerBeehiveLinkFlag && WiFi.isConnected()) {
         registerBeehiveLink();
      }

   }
}

void registerBeehiveLink() {

   if (feebeeCamConfig->setup  && WiFi.isConnected()) {

      BString url =
         BString(PROTOCOL) + "://" + WiFi.localIP().toString().c_str() + "/";

      BeeFishJSONOutput::Object object {
         {"method", "setItem"},
         {"key", "beehiveLink"},
         {"value", url}
      };

      FeebeeCam::BeeFishWebRequest webRequest("/beehive/", "", object);

      webRequest.send();

      if (webRequest.statusCode() == 200) {
         cout << "Response: " << webRequest.response() << endl;
         FeebeeCam::registerBeehiveLinkFlag = false;
      }
      else {
         cout << "Error" << endl;
      }


   }
}

void testWebRequest() {

   WiFi.begin("Laptop", "feebeegeeb3");
   while (!WiFi.isConnected()) {
      Serial.print(".");
      delay(500);
   }
 

}

void initializeStorage() {

   esp_err_t ret = nvs_flash_init();
   if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
   {
      ret = nvs_flash_erase();
      CHECK_ERROR(ret, TAG, "nvs_flash_erase");

      ret = nvs_flash_init();
      CHECK_ERROR(ret, TAG, "nvs_flash_init");
   }
   CHECK_ERROR(ret, TAG, "nvs_flash_init...");

   feebeeCamConfig = new FeebeeCam::Config();
/*
#ifdef DEBUG
   if (!feebeeCamConfig->update("Laptop", "feebeegeeb3", "4db14a0e15e8a6a1bf1eda4dcb5e41c4db7ec311575722b88ac8b3fc0019e2f57ba2518a042f8f6292955f6187f675cee3e94564903faa069194720ff374ca55"))
   {
      ERROR(ESP_FAIL, TAG, "Updating feebeecam config");
   }

#endif
*/
   if (!feebeeCamConfig->load())
      ERROR(ESP_FAIL, TAG, "Failed to load feebeeCamConfig");

}

void initializeLight()
{
   light = new Light;
   light->turnOn();
   delay(10);
   light->turnOff();
}

void initializeBattery() {
   bat_init();
   bat_hold_output();
}

void initializeWeather()
{
   BME280::Settings settings;
   bme = new BME280(settings);
   FeebeeCam::printStatus();
}

namespace FeebeeCam {

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
            .fb_count = FRAME_BUFFER_COUNT,         //if more than one, i2s runs in continuous mode. Use only with JPEG
            .fb_location = CAMERA_FB_IN_PSRAM,
            .grab_mode = CAMERA_GRAB_LATEST
      };

      esp_err_t ret = esp_camera_init(&camera_config);

      CHECK_ERROR(ret, TAG, "Error initializing camera");

      sensor_t *s = esp_camera_sensor_get();

      //initial sensor settings are set in feebee-cam-config.h
   }

   void printStatus()
   {
      BeeFishJSONOutput::Object reading = getWeather();
      cout << reading << endl;
   }


}

void initializeLED() {
   
   led_init(CAMERA_LED_GPIO);
   led_brightness(256);

}

void initializeBeeFish() {
   /*
   esp_err_t ret = ESP_OK;
   ret = beeFishTest();
   if (ret != ESP_OK) {
      while (1) {
         ;
      }
   }
*/
}

#ifdef SECURE_SOCKETS

void initializeSSLCert() {
   
   using namespace httpsserver;

   Serial.println("Creating certificate...");
   
   cert = new SSLCert();
   
   int createCertResult = createSelfSignedCert(
      *cert,
      KEYSIZE_2048,
      "CN=bee.fish,O=bee.fish,C=AU");
   
   if (createCertResult != 0) {
      Serial.printf("Error generating certificate");
      return; 
   }
 
   Serial.println("Certificate created successfully");

}

#endif
