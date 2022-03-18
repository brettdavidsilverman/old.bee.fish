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

#include "test.h"
#include "battery.h"

#include "bme280.h"

#include "bm8563.h"
#include "time.h"
#include "led.h"
#include "network.h"
#include "i2c.h"
#include "light.h"
//#include "neo-pixels.h"
#include "web-request.h"
#include "feebee-cam-config.h"
#include "app_httpd.h"
#include "website.h"
#include <bee-fish.h>

#define TAG "FEEBEECAM"

void testWiFiConnect();
void initializeBeeFish();
void initializeBattery();
void initializeTime();
void initializeLight();
void initializeLED();
void FeebeeCam::initializeCamera();
void initializeStorage();
void initializeWeather();
#ifdef SECURE_SOCKETS
void initializeSSLCert();
#endif
void FeebeeCam::printStatus();
void onGotIPAddress();
void registerBeehiveLink();
void setTime();

bool restart = false;

using namespace FeebeeCam;

//FeebeeCam::WebRequest webRequest("/", "");


//mcp23008_t mcp23008;

#define NTP_SERVER "pool.ntp.org"

// I2C pin definition for M5Stack TimerCam
// RTC BM8563 I2C port
#define BM8563_I2C_SDA 12
#define BM8563_I2C_SCL 14

//BM8563 RTC;

bool timeSet = false;


void setup()
{

   Serial.begin(1500000);

   while (!Serial)
      ;

   Serial.println("Starting up....");
   
   initializeWebSite();
   return;

   initializeBattery();
   initializeTime();

   FeebeeCam::initializeCamera();

   initializeWeather();

   initializeLED();   

   initializeBeeFish();

   initializeLight();


   initializeStorage();

   initializeWiFi();
 
#ifdef SECURE_SOCKETS
   initializeSSLCert();
#endif

   startWebServers();

   

   //bm8563_init();

   INFO(TAG, "FeebeeCam Initialized");
}

void loop()
{
   wifiLoop();
   return;

   static unsigned long lastConnectedTime = 0;
   static unsigned long lastCheckTime = 0;

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
      if (FeebeeCam::onGotIPAddressFlag && WiFi.isConnected()) {
         onGotIPAddress();
      }

   }
}

void onGotIPAddress() {
   registerBeehiveLink();
   FeebeeCam::onGotIPAddressFlag = false;
   led_brightness(0);
   FeebeeCam::printStatus();
}

void registerBeehiveLink() {

   if (feebeeCamConfig && feebeeCamConfig->setup  && WiFi.isConnected()) {

      BString url =
         BString(PROTOCOL) + "://" + WiFi.localIP().toString().c_str() + "/";

      BeeFishJSONOutput::Object object {
         {"method", "setItem"},
         {"key", "beehiveURL"},
         {"value", url}
      };

      FeebeeCam::BeeFishWebRequest webRequest("/beehive/", "", object);

      webRequest.send();

      if (webRequest.statusCode() == 200) {
         cout << "Response: " << webRequest.response() << endl;
      }
      else {
         cout << "Error" << endl;
      }


   }
}

void testWiFiConnect() {

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
   light = createLight();
}

void initializeWeather() {
   BME280::Settings settings;
   bme = new BME280(settings);
}

void initializeBattery() {
   bat_init();
   bat_hold_output();
}

BString getTime() {

   rtc_date_t rtc_date;

   bm8563_getTime(&rtc_date);

   char str_buffer[128];

   sprintf( str_buffer,
            "%02i/%02i/%02i %02i:%02i:%02",
            rtc_date.day, rtc_date.month, rtc_date.year,
            rtc_date.hour, rtc_date.minute, rtc_date.second);


   return str_buffer;

}


void initializeTime() {
   bm8563_init();
}

void setTime()
{
   if (timeSet)
      return;

   Serial.println("Configuring NTP Server");

   // Set ntp time to local
   configTime(10 * 3600, 0, NTP_SERVER);

   rtc_date_t rtc_time;

   // Get local time
   struct tm timeInfo;
   if (getLocalTime(&timeInfo)) {
      Serial.println("Got Time");
      rtc_time.hour = timeInfo.tm_hour;
      rtc_time.minute = timeInfo.tm_min;
      rtc_time.second = timeInfo.tm_sec;

      rtc_time.month = timeInfo.tm_mon + 1;
      rtc_time.day = timeInfo.tm_mday;
      rtc_time.year = timeInfo.tm_year + 1900;

      Serial.println("Setting Time");
      bm8563_setTime(&rtc_time);

      timeSet = true;
   }
   else {
      Serial.println("Error getting time");
   }
   
   return;

   
   time_t mytime;

//   struct tm * ptm;

   time(&mytime); // Get local time in time_t

//   ptm = gmtime(&mytime); // Find out UTC time

//   time_t utctime = mktime(ptm); // Get UTC time as time_t

   std::string stringMyTime(ctime(&mytime));
  // std::string stringUTCTime(ctime(&utctime));

   //printf("\nLocal time %X \n(%s) and UTC Time %X \n(%s)", mytime,  stringMyTime.c_str(), utctime, stringUTCTime.c_str());
   printf("\nUTC Time %X \n(%s)", mytime,  stringMyTime.c_str());
  // return;
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

      if (s)
         Serial.println("Camera Initialized");
      //initial sensor settings are set in feebee-cam-config.h
   }

   void printStatus()
   {
      BeeFishJSONOutput::Object reading = getWeather();
      cout << reading << endl;
   }

   BeeFishJSONOutput::Object getWeather() {

      float temp(0), humidity(0), pressure(0);

      BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
      BME280::PresUnit presUnit(BME280::PresUnit_hPa);

      if (bme)
         bme->read(pressure, temp, humidity, tempUnit, presUnit);

      float fps = framesPerSecond;    
      lastTimeFramesCounted = esp_timer_get_time();
      frameCount = 0;

      BeeFishJSONOutput::Object reading {
         {"temp", temp},
         {"humidity", humidity},
         {"pressure", pressure},
         {"memory", ((float)ESP.getHeapSize() - (float)ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0},
         {"psram", ((float)ESP.getPsramSize() - (float)ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0},
         {"battery", bat_get_voltage()},
         {"framesPerSecond", fps}
//         {"time", getTime()}
      };

      if (WiFi.isConnected()) {
         reading["url"] = BString(PROTOCOL "://") + WiFi.localIP().toString().c_str() + "/";
      }

      //cout << reading << endl;

      return reading;
   }

}

void initializeLED() {
   
   led_init(CAMERA_LED_GPIO);
   led_brightness(256);

}

void initializeBeeFish() {
   //esp_err_t ret = beeFishTest();
   //CHECK_ERROR(ret, TAG, "beeFishTest");
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
