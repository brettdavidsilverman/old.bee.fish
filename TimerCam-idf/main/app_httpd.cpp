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
#include <ESPmDNS.h>

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
#include "website.h"
#include <bee-fish.h>
#include "app_httpd.h"

/* A simple example that demonstrates how to create GET and POST
 * handlers and start an HTTPS server.
*/

static const char *TAG = "example";

httpd_handle_t server = NULL;
httpd_handle_t cameraServer = NULL;

IPAddress IP(10, 10, 1, 1);
IPAddress gatewayIP(10, 10, 1, 1);
IPAddress subnetIP(255, 255, 255, 0);

static bool serversRunning = false;

/* An HTTP GET handler */
static esp_err_t setup_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*)setup_html_start, setup_html_end - setup_html_start);

    return ESP_OK;
}

/* An HTTP GET handler */
static esp_err_t beehive_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char*)beehive_html_start, beehive_html_end - beehive_html_start);

    return ESP_OK;
}


/* An HTTP POST handler */
static esp_err_t settings_post_handler(httpd_req_t *req)
{
    using namespace BeeFishJSON;

    JSONParser::clear();

    JSON json;
    
    JSONParser::OnValue onframesize = 
        [](const BString& key, JSON& json) {
            sensor_t *s = esp_camera_sensor_get();
            
            framesize_t framesize = FRAMESIZE_SVGA;
            
            const BString& value = json.value();

            if (value == "FRAMESIZE_QVGA")
                framesize = FRAMESIZE_QVGA;
            else if (value == "FRAMESIZE_CIF")
                framesize = FRAMESIZE_CIF;
            else if (value == "FRAMESIZE_SVGA")
                framesize = FRAMESIZE_SVGA;
            else if (value == "FRAMESIZE_XGA")
                framesize = FRAMESIZE_XGA;
            else if (value == "FRAMESIZE_QXGA")
                framesize = FRAMESIZE_QXGA;

            s->set_framesize(s, framesize);
            cout << "Set Frame Size " << value << endl;
        };

    JSONParser::invokeValue("framesize", onframesize);

    JSONParser parser(json);
    esp_err_t ret = ESP_OK;

    char buff[4096];
    for (size_t i = 0; i < req->content_len; i += sizeof(buff)) {
        
        size_t readSize = sizeof(buff);

        if (i + readSize > req->content_len)
            readSize = req->content_len - i;

        int read = httpd_req_recv(req, buff, readSize);

        if (read <= 0) {  
            // 0 return value indicates connection closed 
            // Check if timeout occurred 
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                // In case of timeout one can choose to retry calling
                // httpd_req_recv(), but to keep it simple, here we
                // respond with an HTTP 408 (Request Timeout) error
                httpd_resp_send_408(req);
            }
            ret = ESP_FAIL;
            break;
        }
        
        const std::string str(buff, read);
        //Serial.println(str.c_str());
        
        if (parser.read(str) == false) {
            ret = ESP_FAIL;
            break;
        }
        
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    httpd_resp_set_type(req, "application/json");

    if (ret == ESP_OK && parser.result() == true)
        httpd_resp_send(req, "{\"setup\": \"Ok\"}", HTTPD_RESP_USE_STRLEN);
    else
        httpd_resp_send(req, "{\"setup\": \"Fail\"}", HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t setup_post_handler(httpd_req_t *req) {
    return settings_post_handler(req);
}

static esp_err_t weather_get_handler(httpd_req_t* req) {

    float temp(NAN), humidity(NAN), pressure(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    bme->read(pressure, temp, humidity, tempUnit, presUnit);

    char buffer[1024];
    int n = snprintf(buffer, sizeof(buffer), 
        "{\"temp\": %0.2f, \"humidity\": %0.2f, \"pressure\": %0.2f}",
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
        //taskYIELD();
        //yield();

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

static const httpd_uri_t setupGet = {
    .uri       = "/setup",
    .method    = HTTP_GET,
    .handler   = setup_get_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t beehiveGet = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = beehive_get_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t setupPost = {
    .uri       = "/setup",
    .method    = HTTP_POST,
    .handler   = setup_post_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t settingsPost = {
    .uri       = "/settings",
    .method    = HTTP_POST,
    .handler   = settings_post_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t weatherGet = {
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


void stop_webservers();

httpd_ssl_config_t createSSLConfig() {

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    conf.cacert_pem = cacert_pem_start;
    conf.cacert_len = cacert_pem_end - cacert_pem_start;

    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    conf.httpd.core_id = 0;
    //conf1.httpd.lru_purge_enable = true;
    conf.httpd.max_open_sockets = 4;
    conf.httpd.stack_size = 32768;

    return conf;
}

httpd_config_t createHTTPDConfig() {

    httpd_config_t conf = HTTPD_DEFAULT_CONFIG();

    conf.core_id = 1;
    //conf1.httpd.lru_purge_enable = true;
    conf.max_open_sockets = 4;
    conf.stack_size = 32768;
    conf.server_port = 80;

    return conf;
}

esp_err_t start_webservers(void)
{
    if (serversRunning)
        stop_webservers();

    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Starting http main server...");

    httpd_config_t conf1 = createHTTPDConfig();
    conf1.core_id = 0;

    ret = httpd_start(&server, &conf1);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting main http server!");
        return ret;
    }

    httpd_register_uri_handler(server, &setupGet);
    httpd_register_uri_handler(server, &setupPost);
    httpd_register_uri_handler(server, &beehiveGet);
    httpd_register_uri_handler(server, &weatherGet);
    httpd_register_uri_handler(server, &settingsPost);


    ESP_LOGI(TAG, "Starting http camera server...");

    httpd_config_t conf2 = createHTTPDConfig();
    conf2.core_id = 1;
    conf2.server_port += 1;
    conf2.ctrl_port += 1;

    ret = httpd_start(&cameraServer, &conf2);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting camera http server!");
        return ret;
    }

    httpd_register_uri_handler(cameraServer, &camera);
    
    ESP_LOGI(TAG, "Starting DNS for feebeecam.local");
    MDNS.begin("feebeecam");
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("http", "tcp", 81);

    Serial.println("http://feebeecam.local/");
    Serial.println("");
    Serial.println("http://" + WiFi.localIP().toString() + "/");
    Serial.println("http://" + WiFi.localIP().toString() + ":81/camera");
    Serial.println("http://" + WiFi.localIP().toString() + "/weather");
    Serial.println("http://" + WiFi.localIP().toString() + "/setup");
    /*
    Serial.println("");
    Serial.println("http://" + WiFi.softAPIP().toString() + "/");
    Serial.println("http://" + WiFi.softAPIP().toString() + "/camera");
    Serial.println("http://" + WiFi.softAPIP().toString() + ":444/weather");
    */
    serversRunning = true;

    return ret;
}

void stop_webservers()
{
    if (!serversRunning)
        return;

    // Stop the httpd server
    httpd_stop(server);
    httpd_stop(cameraServer);
    server = NULL;
    cameraServer = NULL;
    serversRunning = false;
}
/*
void WiFiClientConnected(WiFiEvent_t event, WiFiEventInfo_t info) 
{
    Serial.println("WiFi AP Client Connected");
    start_webservers();
}

void WiFiClientDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) 
{
    Serial.println("WiFi AP Client Disconnected");
    stop_webservers();
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.println("WiFi got ip");
    Serial.println("IP address: ");
    Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
    start_webservers();
}

void WiFiLostIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print("WiFi lost ip reason: ");
    Serial.println(info.wifi_sta_disconnected.reason);
    stop_webservers();
    ///WiFi.begin(ssid, password);
} 
*/
void initializeWebServers() {
   WiFi.begin(ssid, password);
   while (!WiFi.isConnected())
   {
      //WiFi.begin();
      Serial.print(".");
      delay(500);
   }
   //WiFi.setAutoReconnect(true);
   //WiFi.persistent(true);
   start_webservers();
/*    
    WiFi.onEvent(WiFiClientConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
    WiFi.onEvent(WiFiClientDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
    WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(WiFiLostIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_LOST_IP);

    WiFi.mode(WIFI_AP);  
    WiFi.softAPConfig(IP, gatewayIP, subnetIP);
    
    WiFi.softAP("Feebeecam", password);    
    Serial.println("WiFi Ready as Access Point: Feebeecam");
    Serial.print("Starting wifi for ");
    Serial.println(ssid);

*/
}

