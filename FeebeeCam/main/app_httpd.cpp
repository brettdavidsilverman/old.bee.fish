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
#include "website.h"
#include <bee-fish.h>
#include "../website/files.h"

#ifdef SECURE_SOCKETS

#include "ssl-cert.h"

#define PROTOCOL "https"
#define CREATE_HTTPD_CONFIG createHTTPDSSLConfig
#define HTTPD_START httpd_ssl_start
#define HTTPD_CONFIG httpd_ssl_config_t
extern httpsserver::SSLCert * cert;

#else

#define PROTOCOL "http"
#define CREATE_HTTPD_CONFIG createHTTPDConfig
#define HTTPD_START httpd_start
#define HTTPD_CONFIG httpd_config_t

#endif


#include "feebee-cam-config.h"
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

IPAddress IP(192,168,4,22);
IPAddress subnetIP(255, 255, 255, 0);
IPAddress gatewayIP(192,168,4,9);

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

    res = httpd_resp_set_hdr(req, "Connection", "Close"); //Keep-Alive
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
   
    const BeeFishBString::BString fileName = parts[parts.size() - 1];

    if (fileName.length()) {
        cout << "FileName part: " << fileName << endl;

        if (FeebeeCam::_files.count(fileName) > 0) {
            const FeebeeCam::File& file = FeebeeCam::_files[fileName];
            if (file._contentType == "certificate")
                return httpd_resp_send_404(req);
            else
                return sendFile(req, file);
        }
        else {
            // Send not found
            return httpd_resp_send_404(req);
        }
    }
    else {
        // Root directory. send beehive.html
        const FeebeeCam::File& file = FeebeeCam::_files["beehive.html"];
        return sendFile(req, file);
    }

    // Send error
    return httpd_resp_send_500(req);
}

/* An HTTP GET handler */
static esp_err_t setup_get_handler(httpd_req_t *req)
{
    const FeebeeCam::File& file = FeebeeCam::_files["setup.html"];

    return sendFile(req, file);
}

/* An HTTP GET handler */
static esp_err_t restart_get_handler(httpd_req_t *req)
{
    Serial.println("Restart request");
    

    ESP.restart();
    

    // Should never reach here
    return ESP_FAIL; 
}

/* An HTTP POST handler */
static esp_err_t settings_post_handler(httpd_req_t *req)
{
    esp_err_t res = ESP_OK;

    JSON json;
    JSONParser parser(json);
    BString setting;

    // Framesize
    JSONParser::OnValue onframesize = 
        [&setting](const BString& key, JSON& json) {
            setting = "Frame size";
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
        [&setting](const BString& key, JSON& json) {
            setting = "Gain Ceiling";
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
        [&setting](const BString& key, JSON& json) {
            setting = "AGC Gain";
            sensor_t *s = esp_camera_sensor_get();
            int agc_gain = atoi(json.value());
            esp_err_t result = s->set_agc_gain(s, agc_gain);
            if (result == ESP_OK)
                cout << "Set AGC Gain " << agc_gain << endl;
        };
        
    parser.invokeValue("agc_gain", onagc_gain);


    // Quality
    JSONParser::OnValue onquality = 
        [&setting](const BString& key, JSON& json) {
            setting = "Quality";
            sensor_t *s = esp_camera_sensor_get();
            int quality = atoi(json.value());
            esp_err_t result = s->set_quality(s, quality);
            if (result == ESP_OK)
                cout << "Set Quality " << quality << endl;
        };
        
    parser.invokeValue("quality", onquality);

    // Brightness
    JSONParser::OnValue onbrightness = 
        [&setting](const BString& key, JSON& json) {
            setting = "Brightness";
            sensor_t *s = esp_camera_sensor_get();
            int brightness = atoi(json.value());
            esp_err_t result = s->set_brightness(s, brightness);
            if (result == ESP_OK)
                cout << "Set Brightness " << brightness << endl;
        };
        
    parser.invokeValue("brightness", onbrightness);

    // Contrast
    JSONParser::OnValue oncontrast = 
        [&setting](const BString& key, JSON& json) {
            setting = "Contrast";
            sensor_t *s = esp_camera_sensor_get();
            int contrast = atoi(json.value());
            esp_err_t result = s->set_contrast(s, contrast);
            if (result == ESP_OK)
                cout << "Set Contrast " << contrast << endl;
        };
        
    parser.invokeValue("contrast", oncontrast);

    // Saturation
    JSONParser::OnValue onsaturation = 
        [&setting](const BString& key, JSON& json) {
            setting = "Saturation";
            sensor_t *s = esp_camera_sensor_get();
            int saturation = atoi(json.value());
            esp_err_t result = s->set_saturation(s, saturation);
            if (result == ESP_OK)
                cout << "Set Saturation " << saturation << endl;
        };
        
    parser.invokeValue("saturation", onsaturation);

    res = parseRequest(parser, req);
    
    if (res != ESP_OK)
        return res;

    BeeFishJSONOutput::Object object;
    object["status"] = true;
    object["message"] = BString("Applied new setting to ") + setting;

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
        bool updated = 
            updateWiFiConfig(ssid.value().str().c_str(), password.value().str().c_str());

        if (updated) {
            saveFeebeeCamConfig();
            Serial.println("Updated WiFi config");
            object["status"] = true;
            object["message"] = "Updated WiFi config. You must restart device to continue.";
        }
        else {
            Serial.println("Error updating WiFi config");
            object["status"] = false;
            object["message"] = "Error updating WiFi config";
        }
    }
    
    res = sendResponse(req, object);
    
    CHECK_ERROR(res, TAG, "Error sending setup post response");

    return res;

}


static esp_err_t weather_get_handler(httpd_req_t* req) {

    ESP_LOGI(TAG, "Weather get handler");

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

    //light->turnOn();

    while(true){
        
        //esp_task_wdt_restart();
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

static const httpd_uri_t restartGet = {
    .uri       = "/restart",
    .method    = HTTP_GET,
    .handler   = restart_get_handler,
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

#ifdef SECURE_SOCKETS

httpd_ssl_config_t createHTTPDSSLConfig(int plusPort) {

    cout << "Creating HTTPD SSL Config" << endl;

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

/*
    const FeebeeCam::File& certFile = FeebeeCam::_files["feebeecam.local.crt"];
    const FeebeeCam::File& keyFile = FeebeeCam::_files["feebeecam.local.key"];

    cout << "Certificate file length: " << certFile._length << endl;
    cout << "Certificate key file length: " << keyFile._length << endl;

    conf.cacert_pem = certFile._data;
    conf.cacert_len = certFile._length;

    conf.prvtkey_pem = keyFile._data;
    conf.prvtkey_len = keyFile._length;
*/

    using namespace httpsserver;

    conf.cacert_pem = cert->getCertData();
    conf.cacert_len = cert->getCertLength();

    conf.prvtkey_pem = cert->getPKData();
    conf.prvtkey_len = cert->getPKLength();

    //conf.httpd = createHTTPDConfig();
    conf.httpd.core_id = 1;
    conf.httpd.lru_purge_enable = true;
    conf.httpd.max_open_sockets = 5;
    conf.httpd.stack_size = 8192;
    conf.httpd.uri_match_fn = httpd_uri_match_wildcard;

    conf.port_secure = 443 + plusPort;
    conf.port_insecure = 80 + plusPort;
    conf.httpd.server_port = 0;
    conf.httpd.ctrl_port += plusPort;

    return conf;
}
#else

httpd_config_t createHTTPDConfig(int plusPort) {

    httpd_config_t conf = HTTPD_DEFAULT_CONFIG();

    conf.core_id = 1;
    conf.lru_purge_enable = true;
    conf.max_open_sockets = 4;
    conf.stack_size = 8192;
    conf.server_port = 80 + plusPort;
    conf.ctrl_port += plusPort;
    conf.uri_match_fn = httpd_uri_match_wildcard;

    return conf;
}


#endif

esp_err_t start_webservers(void)
{
    stop_webservers();

    esp_err_t ret = ESP_OK;

    ESP_LOGI(TAG, "Starting " PROTOCOL " main server...");

    HTTPD_CONFIG conf1 = CREATE_HTTPD_CONFIG(0);
 //   conf1.httpd.core_id = 1;

    ret = HTTPD_START(&server, &conf1);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting main " PROTOCOL " server!");
        return ret;
    }

    httpd_register_uri_handler(server, &setupGet);
    httpd_register_uri_handler(server, &setupPost);
    httpd_register_uri_handler(server, &restartGet);
    httpd_register_uri_handler(server, &weatherGet);
    httpd_register_uri_handler(server, &settingsGet);
    httpd_register_uri_handler(server, &settingsPost);
    httpd_register_uri_handler(server, &fileGet);

    ESP_LOGI(TAG, "Starting " PROTOCOL " camera server...");

    HTTPD_CONFIG conf2 = CREATE_HTTPD_CONFIG(1);

    ret = HTTPD_START(&cameraServer, &conf2);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting camera " PROTOCOL " server %i, %s", ret, esp_err_to_name(ret));
        BeeFishJSONOutput::Object memory {
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

    Serial.println(PROTOCOL "://feebeecam.local/");
    Serial.println("");
    Serial.println(PROTOCOL "://" + WiFi.localIP().toString() + "/");
    Serial.println(PROTOCOL "://" + WiFi.localIP().toString() + ":444/camera");
    Serial.println(PROTOCOL "://" + WiFi.localIP().toString() + "/weather");
    Serial.println(PROTOCOL "://" + WiFi.localIP().toString() + "/setup");
    Serial.println("");
    Serial.println(PROTOCOL "://" + WiFi.softAPIP().toString() + "/");
    Serial.println(PROTOCOL "://" + WiFi.softAPIP().toString() + ":444/camera");
    Serial.println(PROTOCOL "://" + WiFi.softAPIP().toString() + "/weather");
    Serial.println(PROTOCOL "://" + WiFi.softAPIP().toString() + "/setup");

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
    //start_webservers();
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
    //start_webservers();
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


    if (strlen(getWiFiSSID()) > 0) {
        Serial.print("WiFi connecting to ");
        Serial.print(getWiFiSSID());

        WiFi.begin(getWiFiSSID(), getWiFiPwd());
        
        uint32_t timer = millis();
        while (!WiFi.isConnected() && ((millis() - timer) < 10000)) {
            Serial.print(".");
            delay(500);
        }
        if (WiFi.isConnected()) {
            Serial.println("Connected");
            start_webservers();
        }
        
    }
    else {
        Serial.println("WiFi connecting to last...");
        WiFi.begin();
    }
    
}

