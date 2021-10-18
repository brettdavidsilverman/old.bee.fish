#include "esp_http_server.h"
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

httpd_handle_t camera_http_handle = NULL;
#ifdef WEATHER
httpd_handle_t weather_http_handle = NULL;
#endif

void startCameraServer();

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
