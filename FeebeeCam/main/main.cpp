#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_camera.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"
#include <Arduino.h>
#include <WiFi.h>

#include "error.h"
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

const char *TAG = "FEEBEECAM";

void initializeLight();
void initializeLED();
void initializeWeather();
void initializeCamera();
void initializeStorage();
#ifdef SECURE_SOCKETS
void initializeSSLCert();
#endif
void printWeatherData();

char CAM_LOGO[] =
      " _____ _                                 ____                        \r\n"
      "|_    _(_)_ __ ___    ___ _ __ / ___|__ _ _ __ ___   \r\n"
      "   | | | | '_ ` _ \\ / _ \\ '__| |    / _` | '_ ` _ \\ \r\n"
      "   | | | | | | | | |   __/ |   | |__| (_| | | | | | |\r\n"
      "   |_| |_|_| |_| |_|\\___|_|    \\____\\__,_|_| |_| |_|\r\n";

bool restart = false;
volatile bool init_finish = false;

//mcp23008_t mcp23008;

void setup()
{

   Serial.begin(115200);

   while (!Serial)
      ;

   Serial.println("Starting up....");
   
   //Serial.println("Clearing Wifi Data");
   //WiFi.disconnect(true);

/*
   esp_err_t ret = ESP_OK;
   ret = beeFishTest();
   if (ret != ESP_OK) {
      while (1) {
         ;
      }
   }
*/
   initializeCamera();
   initializeStorage();
   loadFeebeeCamConfig();
   initializeWeather();
   initializeLED();
#ifdef SECURE_SOCKETS
   initializeSSLCert();
#endif
   initializeWiFi();

   BeeFishJSONOutput::Object object;
   
   object["secret"] = "Hello World";
   object["method"] = "logon";

   FeebeeCam::WebRequest webRequest("bee.fish", "/", object.str(), HTTP_METHOD_POST);

   //start_webservers();
   //Serial.println("Starting WIfi...");
   //WiFi.begin();

   initializeLight();

   Serial.print("Free Heap End:\t");
   Serial.println(ESP.getFreeHeap());

   //Initialize NVS
 
   BeeFishJSONOutput::Object memory {
      {"memory", (float)ESP.getFreeHeap() / (float)ESP.getHeapSize() * 100.0},
      {"psram", (float)ESP.getFreePsram() / (float)ESP.getPsramSize() * 100.0}
   };

   cout << memory << endl;

   /*
   if (beeFishTest() != ESP_OK) {
         printf("Bee Fish Test Failed\n");
         while (1)
               ;
   }
   */
   //bm8563_init();
   //bat_init();
   //bat_hold_output();

   //    esp_log_level_set(TAG, ESP_LOG_ERROR);
   //    printf("%s", CAM_LOGO);

   //InitTimerCamConfig();
   ///InitCamFun();

   //esp_task_wdt_init(1, false);
   //esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(0));

   init_finish = true;
}

unsigned int lastResetTime = 0;
unsigned int lastCheckTime = 0;

void loop()
{
   if (millis() - lastCheckTime > 500) 
   {
      lastCheckTime = millis();

      if ((WiFi.softAPgetStationNum() == 0) && (!WiFi.isConnected()))
      {
         if ((millis() - lastResetTime) > 10000)
         {
            //Serial.println("Restarting WiFi...");
            //initializeWiFi();
            // WiFi.begin(SSID, PASSWORD);
            // WiFi.begin();
            //Serial.println("Reinitializing WiFi...");
            // initializeWiFi();
            Serial.println("Restarting...");
            ESP.restart();
         }

      }
      else {
         lastResetTime = millis();
      }
   }
   delay(10);
}

void initializeStorage() {
   esp_err_t ret = nvs_flash_init();
   if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
   {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
   }
   ESP_ERROR_CHECK(ret);
}

void initializeLight()
{
   light = new Light;
   light->turnOn();
   delay(10);
   light->turnOff();
}

void initializeWeather()
{
   BME280::Settings settings;
   bme = new BME280(settings);
   printWeatherData();
}

void printWeatherData()
{

   float temp(NAN), humidity(NAN), pressure(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_hPa);

   bme->read(pressure, temp, humidity, tempUnit, presUnit);

   printf("Temp, %0.2f°,", temp);
   printf("Humidity, %0.2f%%,", humidity);
   printf("Pressure, %0.2fhPa\n", pressure);
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
         .fb_count = 2         //if more than one, i2s runs in continuous mode. Use only with JPEG
   };

   esp_err_t ret = esp_camera_init(&camera_config);

   CHECK_ERROR(ret, TAG, "Error initializing camera");

   sensor_t *s = esp_camera_sensor_get();

   //initial sensors are flipped vertically and colors are a bit saturated
   /*
   s->set_framesize(s, FRAMESIZE_CIF);
   s->set_quality(s, 5);
   s->set_vflip(s, 1); //flip it back
   s->set_hmirror(s, 1);
   s->set_gainceiling(s, GAINCEILING_16X); //GAINCEILING_2X
   */
}

void initializeLED() {
   
   led_init(CAMERA_LED_GPIO);
   led_brightness(256);

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
 
   Serial.println("Certificate created with success");

}

#endif
