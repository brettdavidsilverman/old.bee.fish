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
#include "esp_https_server.h"
#include "esp_tls.h"
#include "esp_camera.h"

#include "light.h"
#include "neo-pixels.h"
#include "battery.h"
#include "bme280.h"
#include "website.h"
#include <bee-fish.h>
#include "../website/files.h"

#include "feebee-cam-config.h"
#include "app_httpd.h"

#include "web-request.h"

#ifdef SECURE_SOCKETS
httpsserver::SSLCert * cert = nullptr;
#endif

using namespace BeeFishJSON;
using namespace BeeFishParser;
using namespace BeeFishBString;

using namespace FeebeeCam;

volatile bool FeebeeCam::onGotIPAddressFlag = true;
volatile bool FeebeeCam::isRunning = false;
volatile bool FeebeeCam::stop = false;

int64_t         FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
unsigned long   FeebeeCam::frameCount = 0;
float           FeebeeCam::framesPerSecond = 0.0;

bool stopped = false;
httpd_handle_t server = NULL;
httpd_handle_t cameraServer = NULL;

IPAddress FeebeeCam::localIPAddress(192, 168, 4, 2);
IPAddress FeebeeCam::subnetIP(255, 255, 255, 0);
IPAddress FeebeeCam::gatewayIP(192, 168, 4, 1);


/* A simple example that demonstrates how to create GET and POST
 * handlers and start an HTTPS server.
*/

#define TAG "FeebeeCam HTTP Server"

gainceiling_t gainceiling = GAINCEILING_4X;

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

esp_err_t sendCommonHeaders(httpd_req_t* req) {

    esp_err_t res = ESP_OK;

    res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    CHECK_ERROR(res, TAG, "Error set access control allow origin");

    res = httpd_resp_set_hdr(req, "Keep-Alive",  "timeout=10, max=1");
    CHECK_ERROR(res, TAG, "Error set keep alive header");

    res = httpd_resp_set_hdr(req, "Connection",  "Keep-Alive");
    CHECK_ERROR(res, TAG, "Error set connection header");

    return res;
}

esp_err_t sendResponse(httpd_req_t *req, const BeeFishJSONOutput::Object& output) {

    esp_err_t res;

    res = httpd_resp_set_type(req, "application/javascript");
    CHECK_ERROR(res, TAG, "Error set content type");

    res = sendCommonHeaders(req);
    CHECK_ERROR(res, TAG, "sendCommonHeaders()");

    BeeFishBString::DataStream stream;

    stream.setOnBuffer(
        [&res, req](const Data& buffer) {
            if (res == ESP_OK)
                res = httpd_resp_send_chunk(req, (const char*)buffer.data(), buffer.size());
        }
    );

    stream << output;

    stream.flush();

    if (res == ESP_OK) {
        res = httpd_resp_send_chunk(req, nullptr, 0);
    }

    CHECK_ERROR(res, TAG, "Error sending data");

    //INFO(TAG, "Streamed object");
    return res;

}

esp_err_t sendFile(httpd_req_t* req, FeebeeCam::File& file) {

    esp_err_t res = ESP_OK;

    std::string fileName = file._fileName;
    std::string contentType = file._contentType;

    INFO(TAG, "Serving file %s as %s", fileName.c_str(), contentType.c_str());

    if (res == ESP_OK)
        res = httpd_resp_set_type(req, contentType.c_str());
    
    if (res == ESP_OK)
        res = sendCommonHeaders(req);

    if (res == ESP_OK)
        res = httpd_resp_send(req, (const char*)file._data, file._length);

    CHECK_ERROR(res, TAG, "Serve file");

    INFO(TAG, "File served");

    return res;
}

static esp_err_t file_get_handler(httpd_req_t* req) {

    INFO(TAG, "File get \"%s\"", req->uri);

    const BeeFishBString::BString uri(req->uri);
    std::vector<BeeFishBString::BString> parts = uri.split('/');
   
    const BeeFishBString::BString fileName = parts[parts.size() - 1];

    if (fileName.length()) {
        if (FeebeeCam::_files.count(fileName) > 0) {
            FeebeeCam::File& file = FeebeeCam::_files[fileName];
            return sendFile(req, file);
        }
        else {
            // Send not found
            INFO(TAG, "File not found \"%s\"", req->uri);
            return httpd_resp_send_404(req);
        }
    }
    else {
        // Root directory. send beehive.html
        FeebeeCam::File& file = FeebeeCam::_files["beehive.html"];
        return sendFile(req, file);
    }

    // Send error
    return httpd_resp_send_500(req);
}

/* An HTTP GET handler */
static esp_err_t setup_get_handler(httpd_req_t *req)
{
    INFO(TAG, "Setup get handler");

    FeebeeCam::File& file = FeebeeCam::_files["setup.html"];

    return sendFile(req, file);
}

/* An HTTP POST handler */
static esp_err_t restart_post_handler(httpd_req_t *req)
{
    INFO(TAG, "Restart request");
    

    ESP.restart();
    

    // Should never reach here
    return ESP_FAIL; 
}

/* An HTTP POST handler */
static esp_err_t camera_post_handler(httpd_req_t *req)
{
    INFO(TAG, "Camera post handler");

    BeeFishJSONOutput::Object object;
    object["status"] = BeeFishJSONOutput::Null();
    object["message"] = "Invalid command";
    
    // Command
    BString command;
    JSONParser::OnValue oncommand = 
        [&object, &command](const BString& key, JSON& json) {
            command = json.value();
            if (command == "stop") {
                FeebeeCam::stop = true;
                object["status"] = true;
                object["message"] = "Camera stopped";
            }
            else if (command == "save") {
                feebeeCamConfig->save();
                object["status"] = true;
                object["message"] = "Camera settings saved";
            }
            else if (command == "capture") {
                
                // Set stop flag to initiate stop camera stream procecss
                FeebeeCam::stop = true;

                // Whilst were waiting, save the camera settings to temporary
                // so we can recall after changing capture specific settings
                esp_err_t err = esp_camera_save_to_nvs("fbcam");

                CHECK_ERROR(err, TAG, "esp_camera_save_to_nvs");

                // Wait for camera process to halt
                while (FeebeeCam::isRunning) {
                    yield();
                }

                // Set capture specific settings...
                sensor_t *s = esp_camera_sensor_get();

                // Largest frame size?
                err = s->set_framesize(s, FRAMESIZE_QXGA);  //FRAMESIZE_XGA);

                // Flush the frame buffer queue
                for (int i = 0; i < FRAME_BUFFER_COUNT; i++)
                {
                    camera_fb_t* fb = esp_camera_fb_get();
                    if (fb)
                        esp_camera_fb_return(fb);
                }
                
                // Set lights on
                light->flashOn();

                // Take the picture
                camera_fb_t* fb = esp_camera_fb_get();

                // Turn light off
                light->flashOff();

                if (!fb) {
                    CHECK_ERROR(ESP_FAIL, TAG, "Camera capture failed");
                } 

                err = esp_camera_load_from_nvs("fbcam");
                CHECK_ERROR(err, TAG, "esp_camera_load_from_nvs");
                
                object["status"] = true;
                object["message"] = "Took picture";
                object["type"] = "data:image/jpeg;base64,";
                object["base64"] = BeeFishBase64::encode(fb->buf, fb->len);

                if (fb)
                    esp_camera_fb_return(fb);

            }
        };
    
    BeeFishJSON::JSON json;

    BeeFishJSON::JSONParser parser(json);

    parser.invokeValue("command", oncommand);

    esp_err_t res = parseRequest(parser, req);
    
    CHECK_ERROR(res, TAG, "parseRequest");

    res = sendResponse(req, object);

    CHECK_ERROR(res, TAG, "Error sending post camera response");
    
    INFO(TAG, "Sent Camera command \"%s\"", command.c_str());

    return res;
;
    

}

/* An HTTP POST handler */
static esp_err_t settings_post_handler(httpd_req_t *req)
{
    INFO(TAG, "Settings post handler");

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
            
            unsigned int value = atoi(json.value().str().c_str());

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
            unsigned int value = atoi(json.value().str().c_str());
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
            int agc_gain = atoi(json.value().str().c_str());
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
            int quality = atoi(json.value().str().c_str());
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
            int brightness = atoi(json.value().str().c_str());
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
            int contrast = atoi(json.value().str().c_str());
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
            int saturation = atoi(json.value().str().c_str());
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

    INFO(TAG, "Settings get handler");

    using namespace BeeFishJSON;

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

    INFO(TAG, "Setup post handler");

    esp_err_t res = ESP_OK;

    JSON json;
    JSONParser parser(json);


    BeeFishMisc::optional<BString> ssid;
    BeeFishMisc::optional<BString> password;
    BeeFishMisc::optional<BString> secretHash;

    parser.captureValue("ssid", ssid);
    parser.captureValue("password", password);
    parser.captureValue("secretHash", secretHash);

    res = parseRequest(parser, req);
    
    if (res != ESP_OK)
        return res;

    BeeFishJSONOutput::Object object;

    if (parser.result() == true && ssid.hasValue()) {
        std::string _ssid = ssid.value();
        std::string _password;
        if (password.hasValue())
            _password = password.value().str();

        INFO(TAG, "Setting WiFi Config for host \"%s\" with password \"%s\"",
            _ssid,
            _password
        );

        bool updated = 
            feebeeCamConfig->update(ssid, password, secretHash);

        if (updated) {
            feebeeCamConfig->load();
            if (feebeeCamConfig->setup) {
                INFO(TAG, "Updated FeebeeCam config");
                object["status"] = true;
                object["message"] = "Updated FeebeeCam config. You must restart device to continue.";
                object["redirectURL"] = HOST "/beehive/";  
            }
            else {
                INFO(TAG, "Error updating FeebeeCam config");
                object["status"] = false;
                object["message"] = "Error saving feebeecam config.";
            }
        }
        else {
            INFO(TAG, "Error updating WiFi config");
            object["status"] = false;
            object["message"] = "Error updating WiFi config";
        }
    }
    
    res = sendResponse(req, object);
    
    CHECK_ERROR(res, TAG, "Error sending setup post response");

    return res;

}


static esp_err_t weather_get_handler(httpd_req_t* req) {

    //INFO(TAG, "Weather get handler");

    BeeFishJSONOutput::Object reading = getWeather();

    esp_err_t res = sendResponse(req, reading);

    CHECK_ERROR(res, TAG, "Error sending weather get response");

    return res;
}



#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

esp_err_t camera_get_handler(httpd_req_t *req) {
    
    INFO(TAG, "Cemera get handler");
    
    
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len;
    uint8_t * _jpg_buf;
    char * part_buf[64];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    CHECK_ERROR(res, TAG, "Error setting content type");

    res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    CHECK_ERROR(res, TAG, "Error set access control allow origin");

    res = httpd_resp_set_hdr(req, "Cache-Control", "no-store, max-age=0");
    CHECK_ERROR(res, TAG, "Error set cache-control");

    light->turnOn();
    
    FeebeeCam::stop = false;
    FeebeeCam::isRunning = true;

    while(!FeebeeCam::stop){
        
        //esp_task_wdt_restart();
        //taskYIELD();
        //yield();

        fb = esp_camera_fb_get();
        if (!fb) {
            DEBUG_OUT("Camera capture failed");
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

        ++frameCount;

        int64_t frameEndTime = esp_timer_get_time();
        int64_t frameTime = frameEndTime - FeebeeCam::lastTimeFramesCounted;
        FeebeeCam::framesPerSecond =
            1000.00 * 1000.00 * (float)FeebeeCam::frameCount / (float)frameTime;

    }

    httpd_resp_send_chunk(req, nullptr, 0);
    
    FeebeeCam::stop = false;
    FeebeeCam::isRunning = false;

    light->turnOff();
    
    return res;
}

static const httpd_uri_t setupGet = {
    .uri       = "/setup",
    .method    = HTTP_GET,
    .handler   = setup_get_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t restartPost = {
    .uri       = "/restart",
    .method    = HTTP_POST,
    .handler   = restart_post_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t cameraPost = {
    .uri       = "/camera",
    .method    = HTTP_POST,
    .handler   = camera_post_handler,
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

static const httpd_uri_t fileGet = {

    .uri       = "/*",
    .method    = HTTP_GET,
    .handler   = file_get_handler,
    .user_ctx  = nullptr
};

static const httpd_uri_t cameraGet = {
    .uri       = "/camera",
    .method    = HTTP_GET,
    .handler   = camera_get_handler,
    .user_ctx  = nullptr
};


#ifdef SECURE_SOCKETS

httpd_ssl_config_t createHTTPDSSLConfig(int plusPort, int core) {

    cout << "Creating HTTPD SSL Config" << endl;

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    conf.cacert_pem = cert->getCertData();
    conf.cacert_len = cert->getCertLength();

    conf.prvtkey_pem = cert->getPKData();
    conf.prvtkey_len = cert->getPKLength();

    conf.httpd.core_id = core;
    conf.httpd.lru_purge_enable = true;
    conf.httpd.max_open_sockets = 2;
    conf.httpd.stack_size = 16384;
    conf.httpd.uri_match_fn = httpd_uri_match_wildcard;

    conf.port_secure = 443 + plusPort;
    conf.port_insecure = 80 + plusPort;
    conf.httpd.server_port = 0;
    conf.httpd.ctrl_port += plusPort;

    return conf;
}

#else

httpd_config_t createHTTPDConfig(int plusPort, int core) {

    httpd_config_t conf = HTTPD_DEFAULT_CONFIG();

    conf.core_id = core;
    conf.lru_purge_enable = true;
    conf.max_open_sockets = 2;
    conf.stack_size = 32768;
    conf.server_port = 80 + plusPort;
    conf.ctrl_port += plusPort;
    conf.uri_match_fn = httpd_uri_match_wildcard;

    return conf;
}


#endif

void WiFiClientConnected(WiFiEvent_t event, WiFiEventInfo_t info) 
{
    INFO(TAG, "WiFi AP Client Connected");
    //FeebeeCam::startWebServers();
    FeebeeCam::printWebServers();
}

void WiFiClientDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) 
{
    INFO(TAG, "WiFi AP Client Disconnected");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{

    INFO(TAG, "WiFi got ip");
    FeebeeCam::printWebServers();

    FeebeeCam::onGotIPAddressFlag = true;    
}

void WiFiLostIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
    Serial.print("WiFi lost ip reason: ");
    Serial.println(info.wifi_sta_disconnected.reason);
} 

namespace FeebeeCam {

    esp_err_t startWebServers(void)
    {
        FeebeeCam::stopWebServers();

        esp_err_t ret = ESP_OK;

        Serial.println("Starting " PROTOCOL " main server");

        HTTPD_CONFIG mainConfig = CREATE_HTTPD_CONFIG(0, 1);
        //tskNO_AFFINITY

        ret = HTTPD_START(&server, &mainConfig);
        if (ESP_OK != ret) {
            Serial.println("Error starting main " PROTOCOL " server!");
            return ret;
        }

        httpd_register_uri_handler(server, &setupGet);
        httpd_register_uri_handler(server, &setupPost);
        httpd_register_uri_handler(server, &restartPost);
        httpd_register_uri_handler(server, &cameraPost);
        httpd_register_uri_handler(server, &weatherGet);
        httpd_register_uri_handler(server, &settingsGet);
        httpd_register_uri_handler(server, &settingsPost);
        httpd_register_uri_handler(server, &fileGet);

        Serial.println("Starting " PROTOCOL " camera server");

        HTTPD_CONFIG cameraConfig = CREATE_HTTPD_CONFIG(1, 1);

        ret = HTTPD_START(&cameraServer, &cameraConfig);
        if (ESP_OK != ret) {
            cerr << "Error starting camera " << PROTOCOL << " server " << esp_err_to_name(ret) << endl;
            return ret;
        }


        httpd_register_uri_handler(cameraServer, &cameraGet);

        /*
        Serial.println("Starting DNS for feebeecam.local");
        if (!MDNS.begin("feebeecam"))
        {
            Serial.println("Error starting mDNS");
            return ESP_FAIL;
        }
        */

        serversRunning = true;
        Serial.println("Started all web servers");
        return ret;
    }

    void stopWebServers()
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

    void printWebServers() {

        if (WiFi.isConnected()) {
            Serial.println(PROTOCOL "://" + WiFi.localIP().toString() + "/");
            Serial.println(PROTOCOL "://" + WiFi.localIP().toString() + ":81/camera");
            Serial.println(PROTOCOL "://" + WiFi.localIP().toString() + "/weather");
            Serial.println(PROTOCOL "://" + WiFi.localIP().toString() + "/setup");
        }

        if (WiFi.softAPgetStationNum() > 0) {
            Serial.println("");
            Serial.println(PROTOCOL "://" + WiFi.softAPIP().toString() + "/");
            Serial.println(PROTOCOL "://" + WiFi.softAPIP().toString() + ":81/camera");
            Serial.println(PROTOCOL "://" + WiFi.softAPIP().toString() + "/weather");
            Serial.println(PROTOCOL "://" + WiFi.softAPIP().toString() + "/setup");
        }

    }

    void initializeWiFi() {
        

        WiFi.onEvent(WiFiClientConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);
        WiFi.onEvent(WiFiClientDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);
        WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
        WiFi.onEvent(WiFiLostIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_LOST_IP);

        cout << "local IP Address: " << localIPAddress.toString().c_str() << endl;

        WiFi.disconnect(true,true);
        
        WiFi.mode(WIFI_AP_STA);

        WiFi.softAPConfig(localIPAddress, gatewayIP, subnetIP);

        WiFi.softAP(softAPSSID, softAPPassword);
        
        if (feebeeCamConfig && feebeeCamConfig->setup) {

            std::string ssid = feebeeCamConfig->ssid();
            std::string password = feebeeCamConfig->password();

            INFO(TAG, "WiFi connecting to \"%s\" with password \"%s\"",
                ssid.c_str(),
                password.c_str()
            );

            WiFi.begin(
                ssid.c_str(),
                password.c_str()
            );        
        }
        else {
            WiFi.begin();
        }
    }
}

