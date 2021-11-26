/* Simple HTTP + SSL Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_task_wdt.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <Arduino.h>
#include <WiFi.h>

#include "esp_netif.h"
#include "esp_eth.h"
#include "network.h"
#include "esp_camera.h"
#include "esp_https_server.h"
#include "esp_tls.h"

#include "light.h"
#include "bme280.h"
#include "error.h"
#include "certificates.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers and start an HTTPS server.
*/

static const char *TAG = "example";

httpd_handle_t server = NULL;
httpd_handle_t cameraServer = NULL;

IPAddress IP(10, 10, 1, 1);
IPAddress gatewayIP(10, 10, 1, 1);
IPAddress subnetIP(255, 255, 255, 0);

/* An HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "<h1>Hello Secure World!</h1>", HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t weather_get_handler(httpd_req_t* req) {

    float temp(NAN), humidity(NAN), pressure(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    bme->read(pressure, temp, humidity, tempUnit, presUnit);

    char buffer[1024];
    int n = snprintf(buffer, sizeof(buffer), 
        "{\"temp:\" %0.2f, \"humidity\": %0.2f, \"pressure\": %0.2f}",
        temp,
        humidity,
        pressure
    );

    esp_err_t res;

    if (n <= sizeof(buffer)) {

        res = httpd_resp_set_type(req, "application/javascript");

        CHECK_ERROR(res, TAG, "Error set content type for weather");

        res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
        CHECK_ERROR(res, TAG, "Error set access control allow origin");

        res = httpd_resp_send(req, buffer, -1);

        CHECK_ERROR(res, TAG, "Error sending data for weather");
    }
    else {
        // Error Occurred
        res = httpd_resp_send_500(req);
    }

    CHECK_ERROR(res, TAG, "Error sending weather");

    return res;
}

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

esp_err_t camera_get_handler(httpd_req_t *req) {
    
    ESP_LOGI(TAG, "Cemera get handler");
    
    
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    char * part_buf[64];
    static int64_t last_frame = 0;
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    CHECK_ERROR(res, TAG, "Error setting content type");

    res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    CHECK_ERROR(res, TAG, "Error set access control allow origin");

    light->turnOn();

    while(true){
        
        //esp_task_wdt_reset();
        taskYIELD();

        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            break ;
        } 

        _jpg_buf_len = fb->len;
        _jpg_buf = fb->buf;

        if(res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);

            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }

        esp_camera_fb_return(fb);

        if(res != ESP_OK){
            break;
        }

        int64_t fr_end = esp_timer_get_time();
        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        ESP_LOGI(TAG, "MJPG: %uKB %ums (%.1ffps)", (uint32_t)(_jpg_buf_len/1024), (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);

    }

    light->turnOff();
    
    last_frame = 0;
    return res;
}

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t weather = {
    .uri       = "/weather",
    .method    = HTTP_GET,
    .handler   = weather_get_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t camera = {
    .uri       = "/camera",
    .method    = HTTP_GET,
    .handler   = camera_get_handler,
    .user_ctx  = nullptr
};

static esp_err_t start_webservers(void)
{
    esp_err_t ret = ESP_OK;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting https server...");

    httpd_ssl_config_t conf1 = HTTPD_SSL_CONFIG_DEFAULT();

    conf1.cacert_pem = cacert_pem_start;
    conf1.cacert_len = cacert_pem_end - cacert_pem_start;

    conf1.prvtkey_pem = prvtkey_pem_start;
    conf1.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    conf1.httpd.core_id = 0;
    conf1.httpd.lru_purge_enable = true;
    ret = httpd_ssl_start(&server, &conf1);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting https server!");
        return ret;
    }

    // Set URI handlers
    httpd_ssl_config_t conf2 = HTTPD_SSL_CONFIG_DEFAULT();

    conf2.cacert_pem = cacert_pem_start;
    conf2.cacert_len = cacert_pem_end - cacert_pem_start;

    conf2.prvtkey_pem = prvtkey_pem_start;
    conf2.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;


    conf2.httpd.core_id = 1;
    conf2.port_secure += 1;
    conf2.httpd.ctrl_port += 1;
    conf2.httpd.lru_purge_enable = true;
    ret = httpd_ssl_start(&cameraServer, &conf2);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting https camera server!");
        return ret;
    }

    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &weather);
    httpd_register_uri_handler(cameraServer, &camera);
    
    Serial.println("https://" + WiFi.softAPIP().toString() + "/");
    Serial.println("https://" + WiFi.softAPIP().toString() + "/weather");
    Serial.println("https://" + WiFi.softAPIP().toString() + ":444/camera");
    return ret;
}

static void stop_webservers()
{
    // Stop the httpd server
    httpd_ssl_stop(server);
    httpd_ssl_stop(cameraServer);
    server = NULL;
    cameraServer = NULL;
}

void initializeWebServer(const char* ssid, const char* password) {

    WiFi.mode(WIFI_AP);  
    WiFi.softAPConfig(IP, gatewayIP, subnetIP);
    WiFi.softAP(ssid, password);
    
    WiFi.waitForConnectResult();

    Serial.println("WiFi Connected as Access Point");

    start_webservers();
}

