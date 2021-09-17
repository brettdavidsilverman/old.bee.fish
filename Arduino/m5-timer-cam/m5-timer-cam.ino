#include "esp_camera.h"
#include <Arduino.h>
#include <WiFi.h>

#include "camera_pins.h"
#include "battery.h"
#include "led.h"

/*
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

*/

bool psramInitialized = false;

#undef malloc

void* malloc(size_t size) {
  if (psramInitialized)
    return ps_malloc(size);
  else
    return ::heap_caps_malloc(size, MALLOC_CAP_8BIT);
}

//BluetoothSerial SerialBT;


const char* ssid = "Bee";
const char* password = "feebeegeeb3";

void startCameraServer(); 


void setup() {

  Serial.begin(115200); 
  //Serial.setDebugOutput(true);
  Serial.println("Setup...");


  
  //SerialBT.begin("m5-timer-cam"); //Bluetooth device name
  //Serial.println("The device started, now you can pair it with bluetooth!");


  initializeCamera();
  initializeMemory();
  initializeLED();
  initializeWiFi();
  initializeBattery();


  led_brightness(1024);
  
  startCameraServer();

  Serial.print("Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  bat_init();
  bat_hold_output();

  led_brightness(0);
}

uint32_t lastTime = 0;

void loop() {

  if ((millis() - lastTime) >= 5000) {
    
    if (Serial.available())
      ESP.restart();

    lastTime = millis();
    // put your main code here, to run repeatedly:
    if  (WiFi.status() != WL_CONNECTED) {
      Serial.println("Restarting...");
      ESP.restart();
    }
    Serial.print("http://");
    Serial.println(WiFi.localIP());
/*
    SerialBT.println("---------------------");
    SerialBT.print("http://");
    SerialBT.println(WiFi.localIP());
    SerialBT.printf("Battery:     %lu\n", bat_get_voltage());  
    SerialBT.printf("Total heap:  %d\n", ESP.getHeapSize());
    SerialBT.printf("Free heap:   %d\n", ESP.getFreeHeap());
    SerialBT.printf("Used heap:   %.2f%%\n", (float)(ESP.getHeapSize() - ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0);
    SerialBT.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    SerialBT.printf("Free PSRAM:  %d\n", ESP.getFreePsram());
    SerialBT.printf("Used PSRAM:  %.2f%%\n", (float)(ESP.getPsramSize() - ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0);
*/
  }

  //delay(10000);

}


void initializeMemory() {

  if (psramFound()) {

    if (!psramInit()){
      Serial.println("Error initializing PSRAM");
      while (1)
        ;
    }

    psramInitialized = true;
    Serial.println("PSRAM Initialized");    
  }

}

void initializeCamera() {

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
  config.fb_count = 2;
 
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
  s->set_brightness(s, 1);//up the blightness just a bit
  s->set_saturation(s, -2);//lower the saturation

  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

}

void initializeLED() {
  led_init(CAMERA_LED_GPIO);
}

void initializeBattery() {
    bat_init();
}

void initializeWiFi() {
  
  Serial.printf("Connect to %s, %s\r\n", ssid, password);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    led_brightness(1024);
    delay(250);
    led_brightness(0);
    delay(250);
  }
 
}

/*
#include <Arduino.h>
//#include <battery.h>
//#include <bmm8563.h>
#include <camera_index.h>
#include <camera_pins.h>
#include <led.h>

#include <WiFi.h>
#include <Wire.h>
//#include <endian.h>

#define wireClockSpeed 4000000

#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_http_server.h"
#include "esp_timer.h"
//#include "esp_camera.h"
#include "img_converters.h"

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
//#include <RGBConverter.h>

static const uint8_t I2C_SDA = 15;
static const uint8_t I2C_SCL = 13;

const char* ssid = "Bee";//"Telstra044F87";
const char* password = "feebeegeeb3";//"ugbs3e85p5";
const char* host_name = "esp32-take-picture";

static const char *TAG = "example:take_picture";

static camera_config_t camera_config;

#define BYTES_PER_PIXEL 3

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* CONTENT_TYPE = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

void startCameraServer();

static esp_err_t init_camera()
{
    camera_config_t& config = camera_config;

    //initialize the camera
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
    config.fb_count = 2;

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }
    sensor_t * s = esp_camera_sensor_get();
    //initial sensors are flipped vertically and colors are a bit saturated
    s->set_vflip(s, 1);//flip it back
    s->set_hmirror(s, 1);
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation

    //drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_SXGA);

    return ESP_OK;
}
*/

/*
httpd_handle_t camera_http_handle = NULL;

typedef struct {
        size_t size; //number of values used for filtering
        size_t index; //current value index
        size_t count; //value count
        int sum;
        int * values; //array to be filled with values
} ra_filter_t;


byte checkEndTransmission(const char* message) {
  byte result = Wire.endTransmission();
  if (result == 0) {
     //Serial.println("Ok");
  }
  else {
    Serial.print(message);
    Serial.print(" error ");
    Serial.println(result);
  }

  return result;
}

static esp_err_t get_image_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    
    esp_err_t res = ESP_OK;
    char part_buf[64];

    static int64_t last_frame = 0;
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }

    sensor_t * s = esp_camera_sensor_get();
    
    s->set_quality(s, 10);
    s->set_framesize(s, FRAMESIZE_SXGA);

    //pinMode(FLASH_LED, OUTPUT);
    //digitalWrite(FLASH_LED, HIGH);

    fb = esp_camera_fb_get();
    
    //digitalWrite(FLASH_LED, LOW);

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


bool capturing = false;

esp_err_t takePicture() {

    esp_err_t res = ESP_OK;

    camera_fb_t * fb = NULL;
    sensor_t * s = esp_camera_sensor_get();

    //s->reset(s);
    //s->set_pixformat(s, PIXFORMAT_JPEG);
    s->set_framesize(s, FRAMESIZE_QVGA);
    s->set_quality(s, 0);

    //pinMode(FLASH_LED, OUTPUT);
    //digitalWrite(FLASH_LED, HIGH);

    fb = esp_camera_fb_get();
    
    //digitalWrite(FLASH_LED, LOW);

    if (!fb) {
        Serial.println("Camera capture failed");
        res = ESP_FAIL;
    }

    uint8_t* bmp = nullptr;
    size_t bmpLength = 0;

    bool converted = false;
    if (fb)
      converted = frame2bmp(fb, &bmp, &bmpLength);

    if(fb) {
        esp_camera_fb_return(fb);
        fb = NULL;
    }

    if (converted) {
      Serial.println("Convert frame buffer to bmp");
      Serial.print("Sending ");
      Serial.println(bmpLength);
      res = sendBitmap(bmp, bmpLength);

      free(bmp);
    }
    else
      res = 1;

    
  
    Serial.println("Took picture");

    return res;
    
}

const uint32_t chunkSize = 32;

int sendBitmap(uint8_t* bmp, size_t length) {
  int result = 0;

  uint32_t chunkCount = (uint32_t)ceil((float)length / (float)chunkSize);

  for (uint32_t chunk = 0; chunk  < chunkCount;  ++chunk) {

      Wire.beginTransmission(0x08);
        
        uint32_t chunkStart = chunk * chunkSize;
        uint32_t chunkEnd = chunkStart + chunkSize;
        if (chunkEnd > length)
            chunkEnd = length;
        uint32_t chunkLength = chunkEnd - chunkStart;
        Wire.write(bmp + chunkStart,  chunkLength);
      
      if ((result = checkEndTransmission("Data Chunk")) != 0) {
        Serial.print("Broke at chunk ");
        Serial.println(chunk);
        break;
      }
  }

  if (result == 0)
    Serial.println("Image sent");
  else
    Serial.println("Error sending image");

  return result;
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

  if(fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
  }
  
  return res;

}

static esp_err_t get_stream_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    frame_count = 0;
    frame_time_sum = 0;

    while(!capturing) {

      int64_t frame_start =  esp_timer_get_time();
  
      fb = esp_camera_fb_get();
      
      if (!fb) {
          Serial.println("Camera capture failed");
          return ESP_FAIL;
      }

      if (res == ESP_OK) {
        res = streamFrameBuffer(req, fb);
      }

      if(res != ESP_OK){
          Serial.println("Client disconnected.");
          break;
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
    
    return res;
}

void logMemory() {
  log_d("Total heap: %d", ESP.getHeapSize());
  log_d("Free heap: %d", ESP.getFreeHeap());
  log_d("Total PSRAM: %d", ESP.getPsramSize());
  log_d("Free PSRAM: %d", ESP.getFreePsram());
}

*/
/*
void setup()
{

  Serial.begin(115200);
  Serial.println();

  //SerialBT.begin("m5-timer-cam"); //Bluetooth device name

  if(ESP_OK != init_camera()) {
    Serial.println("Failed to initialize camera");
    while (1)
      ;
  }
  Serial.println("Camera initialized");
  
  Serial.printf("Connect to %s, %s\r\n", ssid, password);

  led_init(CAMERA_LED_GPIO);
  //bat_init();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    led_brightness(1024);
    delay(250);
    led_brightness(0);
    delay(250);
  }

  led_brightness(0);

  //WiFi.softAP(ssid, password);

  //IPAddress IP = WiFi.softAPIP();
  //Serial.print("AP IP address: ");
  //Serial.println(IP);

  Serial.println("");
  Serial.println("WiFi connected");

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

  //Wire.begin();
  //Wire.setClock(wireClockSpeed);

  Serial.println("Wire started in master mode.");

  config.server_port = 80;
  Serial.printf("Starting server on port: '%d'\n", config.server_port);
  if (httpd_start(&camera_http_handle, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_http_handle, &get_stream_uri);
    httpd_register_uri_handler(camera_http_handle, &get_image_uri);
    Serial.println("Camera Ready! Use: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    //Serial.print(IP);
    Serial.print("/image");
    Serial.println("");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    //Serial.print(IP);
    Serial.print("/stream");
    Serial.println("");
  }
  else
    Serial.println("Error starting server");


  //bat_init();
  //bat_hold_output();
  led_brightness(1024);

  startCameraServer();

  led_brightness(0);

}


enum Process {
  touchWait,
  touchDown,
  touchUp,
  start,
  header,
  pixels,
  footer
};


static uint32_t lastTimeSinceBatteryCheck = 0;

void loop() {


  if ((millis() - lastTimeSinceBatteryCheck) > 5000) {
  //  SerialBT.printf("Batt %lu\n", bat_get_voltage());
    if  (WiFi.status() != WL_CONNECTED) {
      Serial.println("Restarting...");
      ESP.restart();
    }
    lastTimeSinceBatteryCheck = millis();
  }

  //delay(5000);

  if (!capturing) {
    Wire.requestFrom(0x08, 1);
    Process process = (Process)Wire.read();    // receive a byte as character
    //Serial.println(process);
    if (process == Process::start) {
      capturing = true;
      Serial.println("takePicture()");
      takePicture();
      capturing = false;
    }
  }

  if (false && millis() % 10000 == 0) {
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (!isnan(h) && !isnan(t) && !isnan(f)) {

      // Compute heat index in Fahrenheit (the default)
      float hif = dht.computeHeatIndex(f, h);
      // Compute heat index in Celsius (isFahreheit = false)
      float hic = dht.computeHeatIndex(t, h, false);

      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(t);
      Serial.print(F("°C "));
      Serial.print(f);
      Serial.print(F("°F  Heat index: "));
      Serial.print(hic);
      Serial.print(F("°C "));
      Serial.print(hif);
      Serial.println(F("°F"));
    }
  }

  
}
*/
