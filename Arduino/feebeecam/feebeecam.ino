#include <Arduino.h>
#include <battery.h>
#include <camera_index.h>
#include <camera_pins.h>
#include <bmm8563.h>

//#include <esp_task_wdt.h>

#include <Wire.h>
#include <WiFi.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_http_server.h"
//#include "esp_timer.h"
#include "img_converters.h"

#include "esp_camera.h"

//#define BLUETOOTH
#define PSRAM //(try reincluding malloc)
#define WEB_SERVER
#define WEATHER
#define LIGHT
#define LED


#ifdef WEATHER
#include "bme280i2c.h"
#endif

#ifdef LIGHT
#include "light.h"
#endif

#ifdef BLUETOOTH
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial* SerialBT;
#endif


#ifdef PSRAM

void *operator new (size_t size) noexcept {
  return ps_malloc(size);
}

void operator delete (void* pointer) noexcept {
  free(pointer);
}

void *operator new[] (size_t size) noexcept {
  return ps_malloc(size);
}

void operator delete[] (void * pointer) noexcept {
  free(pointer);
};
/*
#undef malloc

void* malloc(size_t size) {
  return ps_malloc(size);
}
*/
#endif

#ifdef LED
#include <led.h>
#endif

#ifdef PSRAM
void initializeMemory();
#endif
void initializeBattery();
#ifdef LIGHT
void initializeLight();
#endif
void initializeCamera();

#ifdef LED
void initializeLED();
#endif

void initializeWiFi();

#ifdef WEATHER
void initializeWeather();
#endif

#ifdef WEB_SERVER
void initializeWebServer();
#else
void startCameraServer();
#endif

#ifdef WEATHER
void printWeatherData(Stream* client);
#endif

void printCPUData(Stream* client);

//const char* ssid = "Bee";
//const char* password = "feebeegeeb3";
const char* ssid = "Telstra044F87";
const char* password = "ugbs3e85p5";
//#define WDT_TIMEOUT 16

#ifdef WEB_SERVER
httpd_handle_t camera_http_handle = NULL;
#endif

#ifdef LIGHT
Light* light;
#endif 

#ifdef WEATHER
BME280I2C bme;
#endif
uint16_t frameBufferCount = 0;

void startCameraServer();

void setup() {

  //esp_task_wdt_init(WDT_TIMEOUT, true);

//  esp_task_wdt_add(NULL);
#ifdef PSRAM
  initializeMemory();
#endif

  Serial.begin(115200); 

  while(!Serial)
    ;

  //Serial.setDebugOutput(true);
  Serial.println("Setup...");



#ifdef BLUETOOTH
  SerialBT = new BluetoothSerial();  
  SerialBT->begin(String("feebeecam")); //Bluetooth device name
#endif


  initializeBattery();
#ifdef LIGHT
  initializeLight();
#endif

  initializeCamera(2);

#ifdef LED
  initializeLED();
#endif

  initializeWiFi();
#ifdef WEATHER
  initializeWeather();
#endif

#ifdef WEB_SERVER
  initializeWebServer();
#else
  startCameraServer();
#endif

#ifdef LED
  led_brightness(1024);
#endif

  Serial.print("Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  //led_brightness(0);
}

uint32_t lastTime = 0;
uint32_t wdt_trip = 0;

void loop() {

  unsigned long time = millis();
  /*
  if ((time - wdt_trip) >= 2000) {
    // Every 2 seconds
    wdt_trip = time;
    esp_task_wdt_reset();
  }
*/
  if ((time - lastTime) >= 5000) {
    
    // Every 5 seconds
#ifdef BLUETOOTH
    if (SerialBT->available()) {
      SerialBT->print("Restarting...");
      ESP.restart();
    }
#endif

    if (Serial.available()) {
      Serial.print("Restarting...");
      ESP.restart();
    }

    lastTime = time;
    // put your main code here, to run repeatedly:
    if  (WiFi.status() != WL_CONNECTED) {
      Serial.println("Restarting...");
      ESP.restart();
    }
  

    Stream* client;

#ifdef BLUETOOTH  
    client = SerialBT;
#else
    client = &Serial;
#endif

    client->println("-----------------");
#ifdef WEATHER
    printWeatherData(client);
#endif

    printCPUData(client);

  }

  
}


#ifdef PSRAM
void initializeMemory() {

  if (psramFound()) {

    if (!psramInit()){
      Serial.println("Error initializing PSRAM");
      while (1)
        ;
    }

    Serial.println("PSRAM Initialized");    
  }

}
#endif

void initializeCamera(uint16_t frameBufferCount) {

  if (frameBufferCount == ::frameBufferCount)
    return;

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
  config.frame_size = FRAMESIZE_UXGA;
  config.jpeg_quality = 10;
  config.fb_count = frameBufferCount;
 
  // camera init
  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Cam failed: 0x%x", err);
    while (1)
      ;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  s->set_vflip(s, 1);//flip it back
  s->set_hmirror(s, 1);
  s->set_brightness(s, 1);//up the blightness just a bit
  s->set_saturation(s, -2);//lower the saturation

  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_SVGA);

  ::frameBufferCount = frameBufferCount;

}

#ifdef LED
void initializeLED() {
  led_init(CAMERA_LED_GPIO);
}
#endif

void initializeBattery() {
  bat_init();
  bat_hold_output();
}

void initializeWiFi() {
  
  Serial.printf("Connect to %s, %s\r\n", ssid, password);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
#ifdef LED
    led_brightness(1024);
    delay(250);
    led_brightness(0);
#endif
    delay(250);
  }
 
}

#ifdef LIGHT
void initializeLight() {
    light = new Light();
}
#endif

#ifdef WEATHER
void initializeWeather() {
  
  Wire.begin();

  if (!bme.begin())
  {
    Serial.println("Could not find BME280 weather sensor!");
    while (1)
      ;
  }

}

void printWeatherData
(
   Stream* client
)
{
   float temp(NAN), humidity(NAN), pressure(NAN);

   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_Pa);

   bme.read(pressure, temp, humidity, tempUnit, presUnit);

   client->printf("Temp:     %0.2f°\n", temp);
   client->printf("Humidity: %0.2f%%\n", humidity);
   client->printf("Pressure: %0.2fPa\n", pressure / 100.0);

}
#endif

void printCPUData(Stream* client) {

  client->print("http://");
  client->println(WiFi.localIP());
  client->printf("Battery:     %lu\n", bat_get_voltage());  
//  client->printf("Total heap:  %d\n", ESP.getHeapSize());
//  client->printf("Free heap:   %d\n", ESP.getFreeHeap());
  client->printf("Used heap:   %.2f%%\n", (float)(ESP.getHeapSize() - ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0);
//  client->printf("Total PSRAM: %d\n", ESP.getPsramSize());
//  client->printf("Free PSRAM:  %d\n", ESP.getFreePsram());
  client->printf("Used PSRAM:  %.2f%%\n", (float)(ESP.getPsramSize() - ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0);

}

#ifdef WEB_SERVER
void initializeWebServer() {

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();


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

  config.server_port = 80;
  Serial.printf("Starting server on port: '%d'\n", config.server_port);
  if (httpd_start(&camera_http_handle, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_http_handle, &get_stream_uri);
    httpd_register_uri_handler(camera_http_handle, &get_image_uri);
    httpd_register_uri_handler(camera_http_handle, &get_weather_uri);
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
    Serial.print("/weather");
    Serial.println("");
    
  }
  else
    Serial.println("Error starting server");

#ifdef LED
  led_brightness(0);
#endif

}

static const char *TAG = "example:take_picture";

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_CONTENT_END = "multipart/x-mixed-replace;boundary=end";
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* CONTENT_TYPE = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

void setFramesize(String uri) {

    sensor_t * s = esp_camera_sensor_get();

    if (uri.endsWith("?low"))
      s->set_framesize(s, FRAMESIZE_QVGA);
    else if (uri.endsWith("?high"))
      s->set_framesize(s, FRAMESIZE_UXGA);
    else // Medium
      s->set_framesize(s, FRAMESIZE_SVGA);

}

static esp_err_t get_weather_handler(httpd_req_t *req)
{
  float temp(NAN), humidity(NAN), pressure(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme.read(pressure, temp, humidity, tempUnit, presUnit);

  char* json = "{\"temperature\":%0.2f,\"humidity\":%0.2f,\"pressure\":%0.2f}"; 
  char buffer[64];
  snprintf(buffer, sizeof(buffer), json, temp, humidity, pressure / 100.0);
  String response = String(buffer);

  esp_err_t res = ESP_OK;
  
  res = httpd_resp_set_status(req, "200");
  
  snprintf(buffer, sizeof(buffer), "%u", response.length());
  String contentLength(buffer);
  
  if (res == ESP_OK)
    res = httpd_resp_set_hdr(req, "Content-Type", "application/json");
  
  if (res == ESP_OK)
    res = httpd_resp_set_hdr(req, "Content-Length", contentLength.c_str());
  
  if (res == ESP_OK)
    res = httpd_resp_send(req, response.c_str(), response.length());

  return res;
}

static esp_err_t get_image_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;

    esp_err_t res = ESP_OK;
    char part_buf[64];

    static int64_t last_frame = 0;
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }

    initializeCamera(1);

    setFramesize(req->uri);

#ifdef LIGHT
    light->turnOn();
#endif
    delay(100);

    fb = esp_camera_fb_get();

#ifdef LIGHT
    light->turnOff();
#endif
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
        res = httpd_resp_set_hdr(req, "Content-Type", "image/jpeg");
        res = httpd_resp_set_hdr(req, "Content-Length", part_buf);
    }

    if(res == ESP_OK){
        res = httpd_resp_send(req, (const char *)(fb->buf), fb->len);
    }

    if(fb){
        esp_camera_fb_return(fb);
        fb = NULL;
    }

    int64_t fr_end = esp_timer_get_time();

    int64_t frame_time = fr_end - last_frame;
    last_frame = fr_end;
    frame_time /= 1000;
    
    Serial.printf(
      "(%.1ffps)\n",
      1000.0 / (uint32_t)frame_time
    );
    
    return res;
}


int64_t frame_count = 0;
int64_t frame_time_sum = 0;

esp_err_t streamFrameBuffer(httpd_req_t* req, camera_fb_t* fb) {
  
  esp_err_t res = ESP_OK;
  char * part_buf[64];

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
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;

    initializeCamera(2);

    setFramesize(req->uri);

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    frame_count = 0;
    frame_time_sum = 0;
#ifdef LIGHT
    light->turnOn();
#endif

    while(res == ESP_OK) {

      int64_t frame_start =  esp_timer_get_time();
  
      fb = esp_camera_fb_get();
      
      if (!fb) {
          Serial.println("Camera capture failed");
      }

      if (fb) {
        res = streamFrameBuffer(req, fb);
        esp_camera_fb_return(fb);
        fb = NULL;
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

    }
    
    Serial.println("Client disconnected.");
      
  //  httpd_resp_send_chunk(req, _STREAM_CONTENT_END, strlen(_STREAM_CONTENT_END));

#ifdef LIGHT    
    light->turnOff();
#endif

    return res;
}
#endif

void logMemory() {
  log_d("Total heap: %d", ESP.getHeapSize());
  log_d("Free heap: %d", ESP.getFreeHeap());
  log_d("Total PSRAM: %d", ESP.getPsramSize());
  log_d("Free PSRAM: %d", ESP.getFreePsram());
}

