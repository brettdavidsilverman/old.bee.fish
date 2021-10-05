#define PSRAM
//define DISPLAY_SERIAL
//#define RTC
//#define BLUETOOTH
#define CAMERA
//#define WEB_SERVER
#define WEB_SERVER2
#define WEATHER
#define LIGHT
#define WIFI
//#define LED
//#define BATTERY


#include <Arduino.h>
#ifdef RTC
#include <bmm8563.h>
#endif
#include <esp_task_wdt.h>
#include <rom/rtc.h>
#include "soc/rtc_wdt.h"
class Test {
public:
  Test() {
#ifdef RTC
    bmm8563_clearIRQ();
#endif
    rtc_wdt_protect_off();
    rtc_wdt_disable();
    esp_task_wdt_delete(NULL);
  }
};

Test test;

/*
#include "light.h"
#include "memory.h"
#include "BluetoothSerial.h"

BluetoothSerial* SerialBT;

void setup() {

  light = new Light();
  light->turnOn();
  SerialBT = new BluetoothSerial();
  SerialBT->begin("feebeecam");
}

void loop() {
  printCPUData(SerialBT);
  delay(5000);
}

void printCPUData(Stream* client) {

  client->printf("Used heap:   %.2f%%\n", (float)(ESP.getHeapSize() - ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0);
  client->printf("Used PSRAM:  %.2f%%\n", (float)(ESP.getPsramSize() - ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0);

}
*/

#include <Wire.h>
#include "light.h"
#ifdef PSRAM
#include "memory.h"
#endif
#ifdef BATTERY
#include <battery.h>
#endif

#ifdef CAMERA
#include <camera_index.h>
#include <camera_pins.h>
#endif

#ifdef WIFI
#include <WiFi.h>
#endif
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

#ifdef WEB_SERVER2
bool gettingImage = false;
#endif

#ifdef RTC
#include <bmm8563.h>
#endif

#ifdef WEATHER
#include "bme280i2c.h"
#endif

#ifdef LIGHT
#include "light.h"
#endif

#ifdef WEB_SERVER

#include "esp_http_server.h"

#elif defined(WEB_SERVER2)

#include <ESPAsyncWebServer.h>
AsyncWebServer* server;

#endif


#ifdef BLUETOOTH
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial* SerialBT;
#endif

#ifdef LED
#include <led.h>
#endif

#ifdef BATTERY
void initializeBattery();
#endif

#ifdef LIGHT
void initializeLight();
#endif

#ifdef LED
void initializeLED();
#endif

#ifdef WIFI
void initializeWiFi();
#endif

#ifdef WEATHER
bool initializeWeather();
#endif

#if defined(WEB_SERVER)

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
#elif defined(WEB_SERVER2)

bool webServerInitialized = false;
bool initializeWebServer();

framesize_t getFrameSize(const AsyncWebServerRequest* request) {

  //FRAMESIZE_ QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA

  if (request->hasParam("size")) {
    const String& size = request->getParam("size")->value();
    if (size == "very-small")
      return FRAMESIZE_QVGA;
    else if (size == "small")
      return FRAMESIZE_CIF;
    else if (size == "large")
      return FRAMESIZE_XGA;
    else if (size == "very-large")
      return FRAMESIZE_QXGA;
    else // Medium
      return FRAMESIZE_SVGA;
  }
  else {
    return FRAMESIZE_SVGA;
  }

}

#else
void startCameraServer();
#endif

#ifdef WEATHER
void printWeatherData(Stream* client);
#endif

void printCPUData(Stream* client);

const char* ssid = "Bee";
const char* password = "feebeegeeb3";
//const char* ssid = "Telstra044F87";
//const char* password = "ugbs3e85p5";
#define WDT_TIMEOUT 16

#ifdef WEB_SERVER
httpd_handle_t camera_http_handle = NULL;
#ifdef WEATHER
httpd_handle_t weather_http_handle = NULL;
#endif
#endif


#ifdef WEATHER
BME280I2C* bme;
#endif
uint16_t frameBufferCount = 0;
framesize_t frameSize = FRAMESIZE_INVALID;

#ifdef WEB_SERVER
void startCameraServer();
#endif

void setup() {
  
#ifdef LIGHT
  initializeLight();
  light->turnOn();
  Serial.begin(115200); 
#endif

  Serial.setDebugOutput(true);
  Serial.println("Setup...");

#ifdef BATTERY
  initializeBattery();
#endif

#ifdef CAMERA
  initializeCamera(1);
#endif

#ifdef LED
  initializeLED();
#endif

#ifdef WIFI
  initializeWiFi();
#endif

#ifdef WEATHER
  initializeWeather();
#endif


#ifdef BLUETOOTH
  SerialBT = new BluetoothSerial();
  SerialBT->begin("feebeecam"); //Bluetooth device name
#endif

#if defined(WEB_SERVER) || defined(WEB_SERVER2)
  if (!initializeWebServer()) {
    Serial.println("Error initializing web server");
    delay(5000);
    ESP.restart();
  };
#endif

#ifdef LED
  led_brightness(1024);
#endif

#ifdef WIFI
  Serial.println("Ready! Use");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/image");
#endif

  Serial.println("Setup complete");

#ifdef LIGHT
  delay(500);
  light->turnOff();
#endif

  //led_brightness(0);
}

uint32_t lastTime = 0;

#ifdef RTC
uint32_t wdt_trip = 0;
#endif

void loop() {


  unsigned long time = millis();

#ifdef RTC
  if ((time - wdt_trip) >= 2000) {
    // Every 2 seconds
    wdt_trip = time;
    esp_task_wdt_reset();
  }
#endif

  if ((time - lastTime) >= 5000) {
    lastTime = time;
    
#ifdef DISPLAY_SERIAL
#ifdef BLUETOOTH  
    handleLoop(SerialBT);
#endif

    handleLoop(&Serial);
#endif
 
#ifdef WIFI
    if  (WiFi.status() != WL_CONNECTED) {
      Serial.println("Restarting...");
      ESP.restart();
    }
#endif
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

#ifdef WEATHER
  printWeatherData(client);
#endif

  printCPUData(client);

}

void initializeCamera(size_t frameBufferCount) {

  //if (frameBufferCount == ::frameBufferCount)
  //  return;

  esp_camera_deinit();
  
#ifdef WEB_SERVER2
  gettingImage = true;
#endif

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
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size =  FRAMESIZE_UXGA;
  config.jpeg_quality = 5;
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
  //s->set_saturation(s, -2);//lower the saturation
  //drop down frame size for higher initial frame rate
  //s->set_framesize(s, FRAMESIZE_SVGA);

  // Flush the first frame buffer out so the next gets a
  // clean image
  //camera_fb_t* fb = esp_camera_fb_get();

  //if(fb) {
  //  esp_camera_fb_return(fb);
  //  fb = NULL;
  //}

#ifdef WEB_SERVER2
  gettingImage = false;
#endif


  ::frameBufferCount = frameBufferCount;


}

#ifdef LED
void initializeLED() {
  led_init(CAMERA_LED_GPIO);
}
#endif

#ifdef BATTERY
void initializeBattery() {
  bat_init();
  bat_hold_output();
}
#endif

#ifdef WIFI
void initializeWiFi() {
  
  Serial.print("Reset Reason: ");
  Serial.print(rtc_get_reset_reason(0));
  Serial.println();

  WiFi.softAP("feebeecam", "feebeegeebz");

  if ( rtc_get_reset_reason(0) == POWERON_RESET ) {
    Serial.printf("Connect to %s, %s to setup.\r\n", ssid, password);
  }
  else {
    Serial.printf("Connecting to %s, %s\r\n", ssid, password);
  }
  
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
#endif

#ifdef LIGHT
void initializeLight() {
  light = new Light();
}
#endif

#ifdef WEATHER
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
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme->read(pressure, temp, humidity, tempUnit, presUnit);

  client->printf("Temp:     %0.2f°\n", temp);
  client->printf("Humidity: %0.2f%%\n", humidity);
  client->printf("Pressure: %0.2fPa\n", pressure / 100.0);

}
#endif

void printCPUData(Stream* client) {

#ifdef WIFI
  client->print("http://");
  client->println(WiFi.localIP());
#endif

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

#if defined(WEB_SERVER) || defined(WEB_SERVER2)
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* CONTENT_TYPE = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";
#endif

#ifdef WEB_SERVER
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

#ifdef WEATHER
  httpd_uri_t get_weather_uri = {
    .uri       = "/weather",
    .method    = HTTP_GET,
    .handler   = get_weather_handler,
    .user_ctx  = NULL
  };
#endif
  
  webServerInitialized = true;

  configCamera.server_port = 80;
  Serial.printf("Starting server on port: '%d'\n", configCamera.server_port);
  if (httpd_start(&camera_http_handle, &configCamera) == ESP_OK) {
    httpd_register_uri_handler(camera_http_handle, &get_stream_uri);
    httpd_register_uri_handler(camera_http_handle, &get_image_uri);
  }
  else
    webServerInitialized = false;

#ifdef WEATHER
  configWeather.server_port = 88;
  configWeather.ctrl_port = 88;
  if (httpd_start(&weather_http_handle, &configWeather) == ESP_OK) {
    httpd_register_uri_handler(weather_http_handle, &get_weather_uri);
  }
  else
    webServerInitialized = false; 
#endif

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

#ifdef LED
  led_brightness(0);
#endif

  return webServerInitialized;

}

static const char *TAG = "example:take_picture";

#ifdef WEATHER
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
#endif

static esp_err_t get_image_handler(httpd_req_t *req){
  initializeCamera(1);
  framesize_t frameSize = getFrameSize(req->uri);
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, frameSize);

  camera_fb_t * fb = NULL;

  esp_err_t res = ESP_OK;
  char part_buf[64];

  static int64_t last_frame = 0;
  if(!last_frame) {
      last_frame = esp_timer_get_time();
  }

  //framesize_t frameSize = getFrameSize(req->uri);
//    framesize_t frameSize = FRAMESIZE_CIF;


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
      //res = httpd_resp_set_hdr(req, "Content-Type", "image/jpeg");
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
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    uint16_t errorCount = 0;

    initializeCamera(2);
    framesize_t frameSize = getFrameSize(req->uri);
    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize(s, frameSize);

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Connection", "close");

    frame_count = 0;
    frame_time_sum = 0;
    
#ifdef LIGHT
    light->turnOn();
#endif

    while(errorCount < 5) {

      int64_t frame_start =  esp_timer_get_time();
  
      fb = esp_camera_fb_get();
      
      if (!fb) {
        Serial.println("Camera capture failed");
        res = ESP_FAIL;
      }

      if (fb) {
        res = streamFrameBuffer(req, fb);
        esp_camera_fb_return(fb);
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
      else
        ++errorCount;
      
    }

    if (errorCount >= 5)
      Serial.println("Stream camera failed.");
    else
      Serial.println("Client disconnected.");

#ifdef LIGHT    
    light->turnOff();
#endif

    return res;
}
#elif defined(WEB_SERVER2)
bool initializeWebServer() {

  if (webServerInitialized)
     return true;

  server = new AsyncWebServer(80);

  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "Hello, world");
  });

#ifdef WEATHER
  server->on("/weather", HTTP_GET, onWeather);
#endif

  server->on("/image", HTTP_GET, onImage);

  server->on("/stream", HTTP_GET, onStream);

  server->onNotFound(onNotFound);

  server->begin();

  webServerInitialized = true;

  return webServerInitialized;
}

void onNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

#ifdef WEATHER
void onWeather(AsyncWebServerRequest *request) {
  initializeWeather();

  char jsonBuffer[64];
  int code;

  if (weatherInitialized) {
    float temp(NAN), humidity(NAN), pressure(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    bme->read(pressure, temp, humidity, tempUnit, presUnit);

    char* json = "{\"temperature\":%0.2f,\"humidity\":%0.2f,\"pressure\":%0.2f}"; 
    snprintf(jsonBuffer, sizeof(jsonBuffer), json, temp, humidity, pressure);
    code = 200;
  }
  else {
    strcpy(jsonBuffer, "{\"error\": \"Couldn't initialize bme280 sensor.\"}");
    code = 500;
  }

  request->send(code, "application/json", jsonBuffer);
}
#endif

void onImage(AsyncWebServerRequest *request) {
  
  initializeCamera(1);
  framesize_t frameSize = getFrameSize(request);
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, frameSize);

  
  camera_fb_t * fb = NULL;

#ifdef LIGHT
    light->turnOn();
#endif

    delay(100);

    fb = esp_camera_fb_get();

    gettingImage = false;

#ifdef LIGHT
    light->turnOff();
#endif

  if(fb) {
      request->sendChunked("image/jpeg",
        [fb](uint8_t *buffer, size_t maxLen, size_t alreadySent) -> size_t {
          size_t length = 
              ((maxLen + alreadySent) > fb->len) ?
                fb->len - alreadySent :
                maxLen;

          if (length == 0) {
            esp_camera_fb_return(fb);
            return 0;
          }

          memcpy(
            buffer,  
            fb->buf + alreadySent,
            length 
          );

          return length;
        }
      );

    fb = NULL;
  }
  else {
    Serial.println("Camera capture failed");
    request->send(500, "text/plain", "Camera capture failed");
  }

    
}

enum sendingState {
  getFrameBuffer = 0,
  contentType,
  content,
  boundary
};
sendingState currentState = sendingState::contentType;

size_t currentPosition = 0;

camera_fb_t * fb = NULL;
  
AsyncWebServerRequest *request;

void disconnect(const char* reason) {
#ifdef LIGHT
  light->turnOff();
#endif
  if (fb) {
    esp_camera_fb_return(fb);
    fb = nullptr;
  }
  Serial.println(reason);
}

void onStream(AsyncWebServerRequest *req) {
  
  request = req;
  currentState = sendingState::getFrameBuffer;
  currentPosition = 0;
  fb = NULL;

  initializeCamera(2);

  framesize_t frameSize = getFrameSize(request);

  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, frameSize);


#ifdef LIGHT
    light->turnOn();
#endif

    request->onDisconnect(
      []() {
        disconnect("Request disconnected");
      }
    );

    AsyncWebServerResponse* response = request->beginChunkedResponse(
      _STREAM_CONTENT_TYPE,
      []
        (
          uint8_t *buffer, 
          size_t maxLen,
          size_t alreadySent
        ) -> size_t 
      {
        Serial.print("sendChunked {");
        Serial.print("\"currentState\":");
        Serial.print(currentState);
        Serial.print(",\"currentPosition\":");
        Serial.print(currentPosition);
        Serial.println("}");
        
        esp_task_wdt_reset();
        size_t length = 0;

        if (request->client()->disconnected()) {
          disconnect("Client disconnected");
          return 0;
        }


        switch (currentState) {
        case sendingState::getFrameBuffer:
          Serial.print("Getting frame buffer: ");
          fb = esp_camera_fb_get();
          if (fb == nullptr) {
            disconnect("Error getting frame buffer");
            return 0;
          }
          Serial.print("{\"fb-len\":");
          Serial.print(fb->len);
          Serial.print("}");
          currentState = sendingState::contentType;

          Serial.println("Ok");
          // Follow throught to sending content type
        case sendingState::contentType:
          Serial.print("Sending content type: ");
          length = snprintf((char*)buffer, maxLen, CONTENT_TYPE, fb->len);
          currentState = sendingState::content;
          currentPosition = 0;
          Serial.println("Ok");
          return length;
        
        case sendingState::content:
          Serial.print("Sending content: ");
          if (currentPosition + maxLen < fb->len)
          {
            length = maxLen;
          }
          else {
            length = fb->len - currentPosition;
          }
          memcpy(buffer, fb->buf + currentPosition, length);
          currentPosition += length;
          if (currentPosition >= fb->len) {
            currentState = sendingState::boundary;
            Serial.println("Ok");
          }
          return length;
        case sendingState::boundary:
          Serial.print("Sending boundary: ");
          length = strlen(_STREAM_BOUNDARY);
          memcpy(buffer, _STREAM_BOUNDARY, length);
          esp_camera_fb_return(fb);
          fb = nullptr;
          currentState = sendingState::getFrameBuffer;
          Serial.println("Ok");
          return length;
        default:
          disconnect("Invalid currentState");
          return 0;
        }
      }
    );
    
    request->send(response);
    
}


#endif

void logMemory() {
  log_d("Total heap: %d", ESP.getHeapSize());
  log_d("Free heap: %d", ESP.getFreeHeap());
  log_d("Total PSRAM: %d", ESP.getPsramSize());
  log_d("Free PSRAM: %d", ESP.getFreePsram());
}

