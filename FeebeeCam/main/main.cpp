#include <stdio.h>

#include "config.h"
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

#define TAG "FEEBEECAM"

void testWebRequest();
void initializeBeeFish();
void initializeBattery();
void initializeLight();
void initializeLED();
void initializeWeather();
void initializeCamera();
void initializeStorage();
#ifdef SECURE_SOCKETS
void initializeSSLCert();
#endif
void printStatus();

bool restart = false;
volatile bool init_finish = false;

using namespace FeebeeCam;

//FeebeeCam::WebRequest webRequest("/", "");


//mcp23008_t mcp23008;

void setup()
{

   Serial.begin(115200);

   while (!Serial)
      ;

   Serial.print("Starting up....");

   testWebRequest();

   initializeBeeFish();
   initializeBattery();
   initializeCamera();
   initializeStorage();

   initializeWeather();
   initializeLED();
#ifdef SECURE_SOCKETS
   initializeSSLCert();
#endif

   initializeWiFi();

   startWebServers();

   if (WiFi.isConnected() && feebeeCamConfig->getSecretHash().size()) {
      if (FeebeeCam::BeeFishWebRequest::logon()) {
         Serial.println("Logged on");
      }
      else {
         Serial.println("Error logging in");
      }
   }
   else {
      cout << "Deferring logon to bee.fish" << endl;
   }

   initializeLight();
   printStatus();

   /*
      BeeFishJSON::Object getStatus;
   getStatus["method"] = "getStatus";
   webRequest.body() = getStatus.str();

   cout << "Getting login status..." << endl;
   webRequest.send();
*/


   //
   //Serial.println("Starting WIfi...");
   //WiFi.begin();

   //bm8563_init();


   init_finish = true;
}

unsigned long lastConnectedTime = 0;
unsigned long lastCheckTime = 0;

void loop()
{
   if (millis() - lastCheckTime > 1000) 
   {
      lastCheckTime = millis();

      if ( ( WiFi.softAPgetStationNum() > 0 ) || 
           ( WiFi.isConnected() ) ) 
      {
         lastConnectedTime = millis();
      }
      else if ( ( millis() - lastConnectedTime ) > 30000 )
      {
         Serial.println("Restarting...");
         ESP.restart();
      }
   }
   //delay(10);
}

void testWebRequest() {
   WiFi.begin("Laptop", "feebeegeeb3");
   while (!WiFi.isConnected()) {
      Serial.print(".");
      delay(500);
   }
 
   Serial.println("Connected");
   BeeFishJSONOutput::Object object = {
      {"method", "getStatus"}
   };

   FeebeeCam::JSONWebRequest webRequest("https://laptop", "/", "", object.bstr());

   webRequest.send();

   cout << "Result : " << webRequest.statusCode() << endl;

   while (1)
      ; 

      esp_http_client_config_t config = {
            .url = "https://www.howsmyssl.com",
         /*
         .client_cert_pem = (const char*)(cert->getCertData()),
         .client_cert_len = cert->getCertLength(),
         .client_key_pem = (const char*)(cert->getPKData()),
         .client_key_len = cert->getPKLength(),
         .method = HTTP_METHOD_GET,
         */
         //.event_handler = _http_event_handler,
         //.user_data = this,
         .crt_bundle_attach = esp_crt_bundle_attach
         /*
         .transport_type = HTTP_TRANSPORT_OVER_SSL,
         */
         //.skip_cert_common_name_check = true
   };

   esp_http_client_handle_t client = esp_http_client_init(&config);
   cout << "Sending..." << endl;
   esp_err_t err = esp_http_client_perform(client);
   CHECK_ERROR(err, TAG, "http client perform");

   if (err == ESP_OK) {

      cout << "HTTPS Status = " << esp_http_client_get_status_code(client)
           << " content_length = " << esp_http_client_get_content_length(client) << endl;
   }
   else
      cout << "Error" << endl;

   esp_http_client_cleanup(client);

   while (1)
      ;

}

void initializeStorage() {
   esp_err_t ret = nvs_flash_init();
   if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
   {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
   }
   ESP_ERROR_CHECK(ret);

   feebeeCamConfig = new FeebeeCam::Config();

   if (!feebeeCamConfig->load())
      throw std::runtime_error("Failed to load feebeeCamConfig from non volatile storage");
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
   printStatus();
}

void printStatus()
{

   float temp(NAN), humidity(NAN), pressure(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_hPa);

   bme->read(pressure, temp, humidity, tempUnit, presUnit);

   BeeFishJSONOutput::Object object = {
      {"temp", temp},
      {"humidity", humidity},
      {"pressure", pressure},
      {"battery voltage", bat_get_voltage()},
      {"memory", (float)ESP.getFreeHeap() / (float)ESP.getHeapSize() * 100.0},
      {"psram", (float)ESP.getFreePsram() / (float)ESP.getPsramSize() * 100.0},
      {"bytes free", ESP.getFreeHeap()}
   };

   cout << object << endl;
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

   //initial sensor settings are set in feebee-camn-config.h
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
