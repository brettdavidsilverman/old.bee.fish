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

using namespace BeeFishJSON;
using namespace BeeFishParser;
using namespace BeeFishBString;

/* A simple example that demonstrates how to create GET and POST
 * handlers and start an HTTPS server.
*/

static const char *TAG = "example";

gainceiling_t gainceiling = GAINCEILING_4X;

httpd_handle_t server = NULL;
httpd_handle_t cameraServer = NULL;

IPAddress IP(192, 168, 1, 1);
IPAddress subnetIP(255, 255, 255, 0);
IPAddress gatewayIP(192, 168, 1, 1);

static bool serversRunning = false;

static esp_err_t parseRequest(JSONParser& parser, httpd_req_t *req) {
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

    return ret;
}

esp_err_t sendResponse(httpd_req_t *req, const BeeFishJSONOutput::Object& output) {

    stringstream stream;
    stream << output;

    esp_err_t res;

    res = httpd_resp_set_type(req, "application/javascript");

    CHECK_ERROR(res, TAG, "Error set content type");

    res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    CHECK_ERROR(res, TAG, "Error set access control allow origin");

    res = httpd_resp_set_hdr(req, "Connection", "close");
    CHECK_ERROR(res, TAG, "Error set connection header");

    res = httpd_resp_send(req, stream.str().c_str(), stream.str().length());

    CHECK_ERROR(res, TAG, "Error sending data");

    return res;

}

esp_err_t sendFile(httpd_req_t* req, const FeebeeCam::File& file) {

    esp_err_t res = ESP_OK;

    cout << "Serving file " << file._fileName.str() << " as " << file._contentType.str() << endl;

    if (res == ESP_OK)
        res = httpd_resp_set_type(req, file._contentType.str().c_str());
    
    if (res == ESP_OK)
        res = httpd_resp_set_hdr(req, "Connection", "close");

    if (res == ESP_OK)
        res = httpd_resp_send(req, (const char*)file._data, file._length);

    return res;
}

static esp_err_t file_get_handler(httpd_req_t* req) {

    const BeeFishBString::BString uri(req->uri);
    std::vector<BeeFishBString::BString> parts = uri.split('/');
   
    if (parts.size()) {
        
        const BeeFishBString::BString fileName = parts[parts.size() - 1];

        cout << "FileName part: " << fileName << endl;

        if (FeebeeCam::_files.count(fileName) > 0) {
            const FeebeeCam::File& file = FeebeeCam::_files[fileName];
            return sendFile(req, file);

        }
        else {
            return httpd_resp_send_404(req);
        }

    }

    return httpd_resp_send_500(req);
}

/* An HTTP GET handler */
static esp_err_t setup_get_handler(httpd_req_t *req)
{
    const FeebeeCam::File& file = FeebeeCam::_files["setup.html"];

    return sendFile(req, file);
}

static esp_err_t beehive_get_handler(httpd_req_t *req)
{
    const FeebeeCam::File& file = FeebeeCam::_files["beehive.html"];

    return sendFile(req, file);
}

/* An HTTP POST handler */
static esp_err_t settings_post_handler(httpd_req_t *req)
{
    esp_err_t res = ESP_OK;

    JSON json;
    JSONParser parser(json);

    // Framesize
    JSONParser::OnValue onframesize = 
        [](const BString& key, JSON& json) {
        
            sensor_t *s = esp_camera_sensor_get();
            
            framesize_t framesize = FRAMESIZE_SVGA;
            
            unsigned int value = atoi(json.value());

            switch (value) {
                case 1:
                    framesize = FRAMESIZE_QVGA;
                    break;
                case 2:
                    framesize = FRAMESIZE_CIF;
                    break;
                case 3:
                    framesize = FRAMESIZE_SVGA;
                    break;
                case 4:
                    framesize = FRAMESIZE_XGA;
                    break;
                case 5:
                    framesize = FRAMESIZE_QXGA;
                    break;
                default:
                    framesize = FRAMESIZE_SVGA;
                
            }

            esp_err_t res = s->set_framesize(s, framesize);

            if (res == ESP_OK)
                cout << "Set Frame Size " << value << endl;
        };

    parser.invokeValue("framesize", onframesize);

    // Gain Ceiling
    JSONParser::OnValue ongainceiling = 
        [](const BString& key, JSON& json) {
            sensor_t *s = esp_camera_sensor_get();
            unsigned int value = atoi(json.value());
            gainceiling_t gainceiling = (gainceiling_t)value; 
            esp_err_t result = s->set_gainceiling(s, gainceiling);
            if (result == ESP_OK)
                cout << "Set Gain Ceiling " << value << endl;
        };
        
    parser.invokeValue("gainceiling", ongainceiling);

    // AGC Gain
    JSONParser::OnValue onagc_gain = 
        [](const BString& key, JSON& json) {
            sensor_t *s = esp_camera_sensor_get();
            int agc_gain = atoi(json.value());
            esp_err_t result = s->set_agc_gain(s, agc_gain);
            if (result == ESP_OK)
                cout << "Set AGC Gain " << agc_gain << endl;
        };
        
    parser.invokeValue("agc_gain", onagc_gain);


    // Quality
    JSONParser::OnValue onquality = 
        [](const BString& key, JSON& json) {
            sensor_t *s = esp_camera_sensor_get();
            int quality = atoi(json.value());
            esp_err_t result = s->set_quality(s, quality);
            if (result == ESP_OK)
                cout << "Set Quality " << quality << endl;
        };
        
    parser.invokeValue("quality", onquality);

    // Brightness
    JSONParser::OnValue onbrightness = 
        [](const BString& key, JSON& json) {
            sensor_t *s = esp_camera_sensor_get();
            int brightness = atoi(json.value());
            esp_err_t result = s->set_brightness(s, brightness);
            if (result == ESP_OK)
                cout << "Set Brightness " << brightness << endl;
        };
        
    parser.invokeValue("brightness", onbrightness);

    // Contrast
    JSONParser::OnValue oncontrast = 
        [](const BString& key, JSON& json) {
            sensor_t *s = esp_camera_sensor_get();
            int contrast = atoi(json.value());
            esp_err_t result = s->set_contrast(s, contrast);
            if (result == ESP_OK)
                cout << "Set Contrast " << contrast << endl;
        };
        
    parser.invokeValue("contrast", oncontrast);

    // Saturation
    JSONParser::OnValue onsaturation = 
        [](const BString& key, JSON& json) {
            sensor_t *s = esp_camera_sensor_get();
            int saturation = atoi(json.value());
            esp_err_t result = s->set_contrast(s, saturation);
            if (result == ESP_OK)
                cout << "Set Saturation " << saturation << endl;
        };
        
    parser.invokeValue("saturation", onsaturation);

    res = parseRequest(parser, req);
    
    if (res != ESP_OK)
        return res;

    BeeFishJSONOutput::Object object;
    object["status"] = true;
    object["message"] = "Settings set";

    res = sendResponse(req, object);

    CHECK_ERROR(res, TAG, "Error sending post settings response");

    return res;
}

static esp_err_t settings_get_handler(httpd_req_t* req) {

    using namespace BeeFishJSONOutput;

    sensor_t *s = esp_camera_sensor_get();
    framesize_t framesize = s->status.framesize;

    BeeFishJSONOutput::Object settings;

    switch (framesize) {
    case FRAMESIZE_QVGA:
        settings["framesize"] = 1;
        break;
    case FRAMESIZE_CIF:
        settings["framesize"] = 2;
        break;
    case FRAMESIZE_SVGA:
        settings["framesize"] = 3;
        break;
    case FRAMESIZE_XGA:
        settings["framesize"] = 4;
        break;
    case FRAMESIZE_QXGA:
        settings["framesize"] = 5;
        break;
    default:
        settings["framesize"] = 3;
    }
    
    settings["gainceiling"] = s->status.gainceiling;
    settings["agc_gain"] = s->status.agc_gain;
    settings["quality"] = s->status.quality;
    settings["brightness"] = s->status.brightness;
    settings["contrast"] = s->status.contrast;
    settings["saturation"] = s->status.saturation;

    stringstream stream;
    stream << settings;

    esp_err_t res;

    res = sendResponse(req, settings);

    CHECK_ERROR(res, TAG, "Error sending get settings response");

    return res;
}

static esp_err_t setup_post_handler(httpd_req_t *req) {

    esp_err_t res = ESP_OK;

    JSON json;
    JSONParser parser(json);


    BeeFishMisc::optional<BString> ssid;
    BeeFishMisc::optional<BString> password;
    BeeFishMisc::optional<BString> secret;

    parser.captureValue("ssid", ssid);
    parser.captureValue("password", password);
    parser.captureValue("secret", secret);

    res = parseRequest(parser, req);
    
    if (res != ESP_OK)
        return res;

    BeeFishJSONOutput::Object object;

    if (parser.result() == true && ssid.hasValue()) {
        Serial.print("Connecting to WiFi {");
        Serial.printf("%s", ssid.value().str().c_str());
        Serial.print("} with password {");
        if (password.hasValue()) {
            Serial.printf("%s", password.value().str().c_str());
        }
        Serial.print("}");
        //WiFi.disconnect(false, true);
        if (password.hasValue()) {
            
            WiFi.begin(ssid.value().str().c_str(), password.value().str().c_str());
        }
        else
            WiFi.begin(ssid.value().str().c_str());

        int lastTime = millis();
        while (!WiFi.isConnected() && ((millis() - lastTime) < 20000)) {
            delay(500);
            Serial.print(".");
        }
        if (WiFi.isConnected())
        {
            Serial.println("Restarting into new WiFi network");
            object["status"] = true;
            object["message"] = "Connected to WiFi";
            object["forward"] = BString("https://") + BString(WiFi.localIP().toString().c_str()) + BString("/");
        }
        else {
            object["status"] = false;
            object["message"] = "Could not connect to WiFi";
        }
    }
    
    res = sendResponse(req, object);
    
    CHECK_ERROR(res, TAG, "Error sending setup post response");

    return res;

}


static esp_err_t weather_get_handler(httpd_req_t* req) {

    float temp(NAN), humidity(NAN), pressure(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_hPa);

    bme->read(pressure, temp, humidity, tempUnit, presUnit);

    BeeFishJSONOutput::Object reading {
        {"temp", temp},
        {"humidity", humidity},
        {"pressure", pressure},
        {"memory", (float)ESP.getFreeHeap() / (float)ESP.getHeapSize() * 100.0},
        {"psram", (float)ESP.getFreePsram() / (float)ESP.getPsramSize() * 100.0}
    };

    esp_err_t res = sendResponse(req, reading);
    CHECK_ERROR(res, TAG, "Error sending weather get response");

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

static const httpd_uri_t beehiveGet = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = beehive_get_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t fileGet = {

    .uri       = "/*",
    .method    = HTTP_GET,
    .handler   = file_get_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t setupGet = {
    .uri       = "/setup",
    .method    = HTTP_GET,
    .handler   = setup_get_handler,
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

static const httpd_uri_t settingsGet = {
    .uri       = "/settings",
    .method    = HTTP_GET,
    .handler   = settings_get_handler,
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

httpd_config_t createHTTPDConfig() {

    httpd_config_t conf = HTTPD_DEFAULT_CONFIG();

    conf.core_id = 1;
    conf.lru_purge_enable = true;
    conf.max_open_sockets = 1;
    conf.stack_size = 32768;
    conf.server_port = 80;

    return conf;
}

httpd_ssl_config_t createHTTPDSSLConfig() {

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    conf.cacert_pem = cacert_pem_start;
    conf.cacert_len = cacert_pem_end - cacert_pem_start;

    conf.prvtkey_pem = privatekey_pem_start;
    conf.prvtkey_len = privatekey_pem_end - privatekey_pem_start;

    //conf.httpd = createHTTPDConfig();
    conf.httpd.core_id = 1;
    conf.httpd.lru_purge_enable = true;
    conf.httpd.max_open_sockets = 7;
    conf.httpd.stack_size = 16384;

    return conf;
}

esp_err_t start_webservers(void)
{
    stop_webservers();

    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Starting http main server...");

    httpd_ssl_config_t conf1 = createHTTPDSSLConfig();
 //   conf1.httpd.core_id = 1;
    conf1.httpd.uri_match_fn = httpd_uri_match_wildcard;

    ret = httpd_ssl_start(&server, &conf1);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting main https server!");
        return ret;
    }

    httpd_register_uri_handler(server, &beehiveGet);
    httpd_register_uri_handler(server, &setupGet);
    httpd_register_uri_handler(server, &setupPost);
    httpd_register_uri_handler(server, &beehiveGet);
    httpd_register_uri_handler(server, &weatherGet);
    httpd_register_uri_handler(server, &settingsGet);
    httpd_register_uri_handler(server, &settingsPost);
    httpd_register_uri_handler(server, &fileGet);


    ESP_LOGI(TAG, "Starting https camera server...");

    httpd_ssl_config_t conf2 = createHTTPDSSLConfig();

   // conf2.httpd.core_id = 1;
    conf2.port_secure = 444;
    conf2.port_insecure = 81;
    conf2.httpd.server_port = 0;
    conf2.httpd.ctrl_port += 1;

    /*
    conf2.server_port += 1;
    conf2.ctrl_port += 1;
    */

    ret = httpd_ssl_start(&cameraServer, &conf2);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting camera https server %i, %s", ret, esp_err_to_name(ret));
        BeeFishJSONOutput::Object memory {
            {"conf2.httpd.ctrl_port", conf2.httpd.ctrl_port},
            {"memory", (float)ESP.getFreeHeap() / (float)ESP.getHeapSize() * 100.0},
            {"psram", (float)ESP.getFreePsram() / (float)ESP.getPsramSize() * 100.0}
        };

        cout << memory << endl;
        
        return ret;
    }

    httpd_register_uri_handler(cameraServer, &camera);

    ESP_LOGI(TAG, "Starting DNS for feebeecam.local");
    if (!MDNS.begin("feebeecam"))
    {
        cout << "Error starting mDNS" << endl;
        return ESP_FAIL;
    }

    Serial.println("https://feebeecam.local/");
    Serial.println("");
    Serial.println("https://" + WiFi.localIP().toString() + "/");
    Serial.println("https://" + WiFi.localIP().toString() + ":444/camera");
    Serial.println("https://" + WiFi.localIP().toString() + "/weather");
    Serial.println("https://" + WiFi.localIP().toString() + "/setup");
    Serial.println("");
    Serial.println("https://" + WiFi.softAPIP().toString() + "/");
    Serial.println("https://" + WiFi.softAPIP().toString() + ":444/camera");
    Serial.println("https://" + WiFi.softAPIP().toString() + "/weather");
    Serial.println("https://" + WiFi.softAPIP().toString() + "/setup");

    serversRunning = true;

    return ret;
}

void stop_webservers()
{
    Serial.println("Stopping webservers");
    
    // Stop the httpd server
    if (server)
        httpd_stop(server);

    if (cameraServer)
        httpd_stop(cameraServer);
    
    server = NULL;
    cameraServer = NULL;
    
    serversRunning = false;
}

void WiFiClientConnected(WiFiEvent_t event, WiFiEventInfo_t info) 
{
    Serial.println("WiFi AP Client Connected");
    start_webservers();
}

void WiFiClientDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) 
{
    Serial.println("WiFi AP Client Disconnected");
 //   initializeWiFi();
//    WiFi.reconnect();
//    stop_webservers();
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
   // initializeWiFi();
  //  stop_webservers();
    ///WiFi.begin(ssid, password);
} 

void initializeWiFi() {
    

    WiFi.onEvent(WiFiClientConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
    WiFi.onEvent(WiFiClientDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
    WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
    WiFi.onEvent(WiFiLostIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_LOST_IP);

    WiFi.mode(WIFI_AP_STA);

    WiFi.softAPConfig(IP, gatewayIP, subnetIP);

    WiFi.softAP(softAPSSID, softAPPassword);

    //WiFi.begin(SSID, PASSWORD);
    Serial.println("WiFi connecting to last...");
    WiFi.begin();

}

