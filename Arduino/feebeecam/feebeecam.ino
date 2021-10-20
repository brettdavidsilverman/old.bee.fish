//#define PSRAM
//#define THREADS
//#define DISPLAY_SERIAL
//#define RTC
//#define BLUETOOTH
#define WDT
//#define BATTERY


#include <Arduino.h>
#ifdef RTC
#include <bmm8563.h>
#endif
#ifdef WDT
#include <esp_task_wdt.h>
#include <rom/rtc.h>
#endif


#include <Wire.h>
#include "light.h"
#ifdef BATTERY
#include <battery.h>
#endif

#include <camera_index.h>
#include <camera_pins.h>

#include <WiFi.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "esp_timer.h"
//#include "img_converters.h"

#include "esp_camera.h"
camera_fb_t * fb = nullptr;

#ifdef RTC
#include <bmm8563.h>
#endif

#include "bme280i2c.h"

#include "light.h"

#include "esp_http_server.h"

#ifdef BLUETOOTH
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial* SerialBT;
#endif

#include <led.h>

#ifdef BATTERY
void initializeBattery();
#endif

void initializeLight();

void initializeLED();

void initializeWiFi();

bool initializeWeather();

bool webServerInitialized = false;
bool initializeWebServer();

framesize_t getFrameSize(const String& uri) {
  //FRAMESIZE_ QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA

  if (uri.indexOf("size=very-small") > 0)
    return FRAMESIZE_QVGA;
  else if (uri.indexOf("size=small") > 0)
    return FRAMESIZE_CIF;
  else if (uri.indexOf("size=large") > 0)
    return FRAMESIZE_XGA;
  else if (uri.indexOf("size=very-large") > 0)
    return FRAMESIZE_QXGA;
  else // Medium
    return FRAMESIZE_SVGA;

}

void startCameraServer();
void printWeatherData(Stream* client);
void printCPUData(Stream* client);

const char* ssid = "Bee";
const char* password = "feebeegeeb3";
//const char* ssid = "Telstra044F87";
//const char* password = "ugbs3e85p5";
#ifdef WDT
#define WDT_TIMEOUT 8
#endif

httpd_handle_t camera_http_handle = NULL;
httpd_handle_t weather_http_handle = NULL;


BME280I2C* bme;
uint16_t frameBufferCount = 0;
framesize_t frameSize = FRAMESIZE_INVALID;

void startCameraServer();

#ifdef WDT
unsigned long webserver_wdt_trip = 0;
#endif
unsigned long webServerTime = 0;

void webServerLoop() {

    unsigned long time = millis();
#ifdef WDT
    if ((time - webserver_wdt_trip) >= 2000) {
      // Every 2 seconds
      webserver_wdt_trip = time;

      esp_task_wdt_reset();
    }
#endif

    if ((time - webServerTime) >= 5000) {
      webServerTime = time;
    }


}

void webServerSetup(void* parameter = nullptr) {
#ifdef WDT
  esp_task_wdt_add(NULL);
#endif

  if (!initializeWebServer()) {
    Serial.println("Error initializing web server");
    delay(5000);
    ESP.restart();
  };
  Serial.println("Web Server Started");

#ifdef THREADS
  for (;;) {
    webServerLoop();
    vTaskDelay(10);
  }
#endif
}

void setup() {
  
#ifdef WDT
  esp_task_wdt_init(10, true);
  esp_task_wdt_add(NULL);
#endif 

  initializeLight();
  light->turnOn();

  Serial.begin(115200); 

  Serial.setDebugOutput(true);
  Serial.println("Setup...");

#ifdef BATTERY
  initializeBattery();
#endif

  initializeCamera(2, FRAMESIZE_CIF);

  initializeLED();

  initializeWiFi();

  initializeWeather();


#ifdef BLUETOOTH
  SerialBT = new BluetoothSerial();
  SerialBT->begin("feebeecam"); //Bluetooth device name
#endif

#ifdef THREADS
  xTaskCreatePinnedToCore(
      webServerSetup, /* Function to implement the task */
      "WebServerTask", /* Name of the task */
      30000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      NULL,  /* Task handle. */
      0); /* Core where the task should run */
#else
   webServerSetup();
#endif

  led_brightness(1024);

  Serial.println("Ready! Use");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/image");

  Serial.println("Setup complete");

  light->turnOff();


  //led_brightness(0);
}

unsigned long lastTime = 0;
#ifdef WDT
unsigned long wdt_trip = 0;
#endif

void loop() {


  unsigned long time = millis();
#ifdef WDT

  if ((time - wdt_trip) >= 2000) {
    // Every 2 seconds
    wdt_trip = time;
    esp_task_wdt_reset();
  }
#endif

  if ((time - lastTime) >= 5000) {
    lastTime = time;
    
#ifdef BLUETOOTH  
#ifdef DISPLAY_SERIAL
    handleLoop(SerialBT);
#endif
    if (SerialBT->available()) {
      String text = SerialBT->readString();
      SerialBT->println(text);

      if ( (text == "restart") || 
           (text == "reset"  ) || 
           (text == "reboot" ) ) 
      {
        ESP.restart();
      }
    }
#endif

#ifdef DISPLAY_SERIAL
    handleLoop(&Serial);
#endif
 
    if  (WiFi.status() != WL_CONNECTED) {
      Serial.println("Restarting...");
      ESP.restart();
    }
  }

  
}

void handleLoop(Stream* client) {

  if (client->available()) {
    String command = client->readString();
    command.trim();
    command.toLowerCase();
    if (command == "restart") {
      client->println("Restarting...");
      ESP.restart();
    }
  }

  client->println("-----------------");

  printWeatherData(client);
  printCPUData(client);

}

bool cameraInitialized = false;

void initializeCamera(size_t frameBufferCount, framesize_t frameSize) {

  //frameBufferCount = 1;
  
  if ( cameraInitialized && 
      frameBufferCount == ::frameBufferCount && 
      frameSize == ::frameSize )
    return;

  if ( !cameraInitialized || frameBufferCount != ::frameBufferCount) {

    if (cameraInitialized)
      esp_camera_deinit();
    
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size =  FRAMESIZE_UXGA;
    config.jpeg_quality = 0;
    config.fb_count = frameBufferCount;
  
    // camera init
    esp_err_t err = esp_camera_init(&config);

    if (err != ESP_OK) {
      Serial.printf("Cam failed: 0x%x", err);
      return;
    }

    cameraInitialized = true;
  }
  
  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  s->set_quality(s, 5);
  s->set_vflip(s, 1);//flip it back
  s->set_hmirror(s, 1);
  s->set_framesize(s, frameSize);
  //s->set_contrast(s, -2);
  
  if (fb) {
    esp_camera_fb_return(fb);
    fb = nullptr;
  }

  if (frameSize != ::frameSize) {
    for (int i = 0; i < 2; ++i)
    {
      fb = esp_camera_fb_get();
      if (fb) {
        esp_camera_fb_return(fb);
        fb = nullptr;
      }
    }
  }

  ::frameBufferCount = frameBufferCount;
  ::frameSize = frameSize;

}

void initializeLED() {
  led_init(CAMERA_LED_GPIO);
}

#ifdef BATTERY
void initializeBattery() {
  bat_init();
  bat_hold_output();
}
#endif

void initializeWiFi() {
  
#ifdef WDT
  Serial.print("Reset Reason: ");
  Serial.print(rtc_get_reset_reason(0));
  Serial.println();
#endif

  //WiFi.softAP("feebeecam", "feebeegeebz");

  Serial.printf("Connecting to %s, %s\r\n", ssid, password);
  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    led_brightness(1024);
    delay(250);
    led_brightness(0);
    delay(250);
  }
  
 
}

void initializeLight() {
  light = new Light();
}

bool weatherInitialized = false;

bool initializeWeather() {
  

  if (weatherInitialized)
     return true;

  bme = new BME280I2C();

  Wire.begin();

  weatherInitialized = bme->begin();

  if (!weatherInitialized)
  {
    Serial.println("Could not find BME280 weather sensor!");
  }

  return weatherInitialized;

}

void printWeatherData
(
   Stream* client
)
{
  if (!initializeWeather()) {
    client->printf("Error initializing weather bme280.");
    return;
  }

  float temp(NAN), humidity(NAN), pressure(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  bme->read(pressure, temp, humidity, tempUnit, presUnit);

  client->printf("Temp:     %0.2f°\n", temp);
  client->printf("Humidity: %0.2f%%\n", humidity);
  client->printf("Pressure: %0.2fhPa\n", pressure);

}

void printCPUData(Stream* client) {

  client->print("http://");
  client->println(WiFi.localIP());

#ifdef BATTERY
  client->printf("Battery:     %lu\n", bat_get_voltage());  
#endif

//  client->printf("Total heap:  %d\n", ESP.getHeapSize());
//  client->printf("Free heap:   %d\n", ESP.getFreeHeap());
  client->printf("Used heap:   %.2f%%\n", (float)(ESP.getHeapSize() - ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0);
//  client->printf("Total PSRAM: %d\n", ESP.getPsramSize());
//  client->printf("Free PSRAM:  %d\n", ESP.getFreePsram());
  client->printf("Used PSRAM:  %.2f%%\n", (float)(ESP.getPsramSize() - ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0);

}

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* CONTENT_TYPE = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static esp_err_t get_image_handler(httpd_req_t *req);
static esp_err_t get_stream_handler(httpd_req_t *req);
static esp_err_t get_weather_handler(httpd_req_t *req);


bool initializeWebServer() {

  if (webServerInitialized)
     return true;

  httpd_config_t configCamera = HTTPD_DEFAULT_CONFIG();
  httpd_config_t configWeather  = HTTPD_DEFAULT_CONFIG();


  httpd_uri_t get_image_uri = {
    .uri       = "/image",
    .method    = HTTP_GET,
    .handler   = get_image_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t get_stream_uri = {
    .uri       = "/stream",
     .method    = HTTP_GET,
    .handler   = get_stream_handler,
    .user_ctx  = NULL
  };

  httpd_uri_t get_weather_uri = {
    .uri       = "/weather",
    .method    = HTTP_GET,
    .handler   = get_weather_handler,
    .user_ctx  = NULL
  };
  
  webServerInitialized = true;

  configCamera.server_port = 80;
  Serial.printf("Starting server on port: '%d'\n", configCamera.server_port);
  if (httpd_start(&camera_http_handle, &configCamera) == ESP_OK) {
    httpd_register_uri_handler(camera_http_handle, &get_stream_uri);
    httpd_register_uri_handler(camera_http_handle, &get_image_uri);
  }
  else
    webServerInitialized = false;

  configWeather.server_port = 88;
  configWeather.ctrl_port = 88;
  if (httpd_start(&weather_http_handle, &configWeather) == ESP_OK) {
    httpd_register_uri_handler(weather_http_handle, &get_weather_uri);
  }
  else
    webServerInitialized = false; 

  if (webServerInitialized) {
    Serial.println("Camera Ready! Use: ");

    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.print("/image");
    Serial.println("");

    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.print("/stream");
    Serial.println("");

    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.print(":88/weather");
    Serial.println("");
    
  }
  else {
    Serial.println("Error starting server");
  }

  led_brightness(0);

  return webServerInitialized;

}

static const char *TAG = "example:take_picture";

static esp_err_t get_weather_handler(httpd_req_t *req)
{
  initializeWeather();

  char jsonBuffer[64];
  if (weatherInitialized) {
    float temp(NAN), humidity(NAN), pressure(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    bme->read(pressure, temp, humidity, tempUnit, presUnit);

    char* json = "{\"temperature\":%0.2f,\"humidity\":%0.2f,\"pressure\":%0.2f}"; 
    snprintf(jsonBuffer, sizeof(jsonBuffer), json, temp, humidity, pressure);
  }
  else {
    strcpy(jsonBuffer, "{\"error\": \"Couldn't initialize bme280 sensor.\"}");
  }

  esp_err_t res = ESP_OK;
  
  res = httpd_resp_set_status(req, "200");
  char contentLength[64];
  snprintf(contentLength, sizeof(contentLength), "%u", strlen(jsonBuffer));
  
  if (res == ESP_OK)
    res = httpd_resp_set_hdr(req, "Content-Type", "application/json");
  
  if (res == ESP_OK)
    res = httpd_resp_set_hdr(req, "Content-Length", contentLength);
  
  if (res == ESP_OK)
    res = httpd_resp_send(req, jsonBuffer, strlen(jsonBuffer));

  return res;
}

static esp_err_t get_image_handler(httpd_req_t *req){

  light->turnOn();

  framesize_t frameSize = getFrameSize(req->uri);
  initializeCamera(1, frameSize);

  camera_fb_t * fb = nullptr;

  esp_err_t res = ESP_OK;
  char part_buf[64];

  if(fb){
      esp_camera_fb_return(fb);
      fb = nullptr;
  }

  fb = esp_camera_fb_get();

  light->turnOff();

  if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
  }

    
  if(res == ESP_OK) {
    res = httpd_resp_set_status(req, "200");
    res = httpd_resp_set_type(req, "image/jpeg");
  }
  
  if(res == ESP_OK){
      snprintf((char *)part_buf, 64, "%u", fb->len);
      Serial.print("Content-Length: ");
      Serial.printf(part_buf);
      Serial.println("");
      //res = httpd_resp_set_hdr(req, "Content-Type", "image/jpeg");
      res = httpd_resp_set_hdr(req, "Content-Length", part_buf);
  }

  if(res == ESP_OK){
      res = httpd_resp_send(req, (const char *)(fb->buf), fb->len);
  }

  if(fb){
      esp_camera_fb_return(fb);
      fb = nullptr;
  }
  
  return res;
}


int64_t frame_count = 0;
int64_t frame_time_sum = 0;

esp_err_t streamFrameBuffer(httpd_req_t* req, camera_fb_t* fb) {
  
  esp_err_t res = ESP_OK;
  char * part_buf[64];

  if (fb == nullptr)
     return ESP_FAIL;

  if(res == ESP_OK){
    size_t hlen = snprintf((char *)part_buf, 64, CONTENT_TYPE, fb->len);
    res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
  }

  if(res == ESP_OK){
    res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
  }

  if(res == ESP_OK){
    res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
  }

  return res;

}


static esp_err_t get_stream_handler(httpd_req_t *req){

    light->turnOn();

    if(fb){
        esp_camera_fb_return(fb);
        fb = nullptr;
    }
    esp_err_t res = ESP_OK;
    uint16_t errorCount = 0;

    framesize_t frameSize = getFrameSize(req->uri);
    initializeCamera(2, frameSize);

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Connection", "close");

    frame_count = 0;
    frame_time_sum = 0;
    
    while(errorCount < 5) {

      esp_task_wdt_reset();
      //yield();

      int64_t frame_start =  esp_timer_get_time();
  
      fb = esp_camera_fb_get();
      
      if (!fb) {
        Serial.println("Camera capture failed");
        res = ESP_FAIL;
      }

      if (fb) {
        res = streamFrameBuffer(req, fb);
        esp_camera_fb_return(fb);
        fb = nullptr;
      }
      
      int64_t frame_end = esp_timer_get_time();
  
      int64_t frame_time = frame_end - frame_start;
      frame_time_sum += frame_time;
      if (++frame_count == 10) {
        Serial.printf(
          "(%.1ffps)\n",
          (float)frame_count / ((float)frame_time_sum / 1000.0 / 1000.0)
        );
        frame_time_sum = 0;
        frame_count = 0;
      }
      

      if (res == ESP_OK)
        errorCount = 0;
      else {
        Serial.print("Error ");
        Serial.println(res);
        ++errorCount;
      }
      
    }

    if (errorCount >= 5)
      Serial.println("Stream camera failed.");
    else
      Serial.println("Client disconnected.");

    light->turnOff();

    return res;
}
