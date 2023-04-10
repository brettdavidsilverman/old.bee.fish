// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "esp_timer.h"
#include "esp_http_server.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "fb_gfx.h"
#include "driver/ledc.h"
//#include "camera_index.h"
#include "sdkconfig.h"
//#include "camera_index.h"
#include <feebee-cam.h>

using namespace BeeFishBString;

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "camera_httpd";
#endif

typedef struct {
    httpd_req_t *req;
    size_t len;
} jpg_chunking_t;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: "
    "%d.%06d\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;


typedef struct {
    size_t size;   // number of values used for filtering
    size_t index;  // current value index
    size_t count;  // value count
    int sum;
    int *values;  // array to be filled with values
} ra_filter_t;

static ra_filter_t ra_filter;

static ra_filter_t *ra_filter_init(ra_filter_t *filter, size_t sample_size) {
    memset(filter, 0, sizeof(ra_filter_t));

    filter->values = (int *)malloc(sample_size * sizeof(int));
    if (!filter->values) {
        return NULL;
    }
    memset(filter->values, 0, sample_size * sizeof(int));

    filter->size = sample_size;
    return filter;
}

static int ra_filter_run(ra_filter_t *filter, int value) {
    if (!filter->values) {
        return value;
    }
    filter->sum -= filter->values[filter->index];
    filter->values[filter->index] = value;
    filter->sum += filter->values[filter->index];
    filter->index++;
    filter->index = filter->index % filter->size;
    if (filter->count < filter->size) {
        filter->count++;
    }
    return filter->sum / filter->count;
}

#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
void enable_led(bool en) {  // Turn LED On or Off
    int duty = en ? led_duty : 0;
    if (en && isStreaming && (led_duty > CONFIG_LED_MAX_INTENSITY)) {
        duty = CONFIG_LED_MAX_INTENSITY;
    }
    ledc_set_duty(CONFIG_LED_LEDC_SPEED_MODE, CONFIG_LED_LEDC_CHANNEL, duty);
    ledc_update_duty(CONFIG_LED_LEDC_SPEED_MODE, CONFIG_LED_LEDC_CHANNEL);
    ESP_LOGI(TAG, "Set LED intensity to %d", duty);
}
#endif

static esp_err_t bmp_handler(httpd_req_t *req) {
    camera_fb_t *fb   = NULL;
    esp_err_t res     = ESP_OK;
    uint64_t fr_start = esp_timer_get_time();
    fb                = esp_camera_fb_get();
    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/x-windows-bmp");
    httpd_resp_set_hdr(req, "Content-Disposition",
                       "inline; filename=capture.bmp");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    char ts[32];
    snprintf(ts, 32, "%ld.%06ld", fb->timestamp.tv_sec, fb->timestamp.tv_usec);
    httpd_resp_set_hdr(req, "X-Timestamp", (const char *)ts);

    uint8_t *buf   = NULL;
    size_t buf_len = 0;
    bool converted = frame2bmp(fb, &buf, &buf_len);
    esp_camera_fb_return(fb);
    if (!converted) {
        ESP_LOGE(TAG, "BMP Conversion failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    res = httpd_resp_send(req, (const char *)buf, buf_len);
    free(buf);
    uint64_t fr_end = esp_timer_get_time();
    ESP_LOGI(TAG, "BMP: %llums, %uB", (uint64_t)((fr_end - fr_start) / 1000),
             buf_len);
    return res;
}

static size_t jpg_encode_stream(void *arg, size_t index, const void *data,
                                size_t len) {
    jpg_chunking_t *j = (jpg_chunking_t *)arg;
    if (!index) {
        j->len = 0;
    }
    if (httpd_resp_send_chunk(j->req, (const char *)data, len) != ESP_OK) {
        return 0;
    }
    j->len += len;
    return len;
}

static esp_err_t capture_handler(httpd_req_t *req) {
    camera_fb_t *fb  = NULL;
    esp_err_t res    = ESP_OK;
    int64_t fr_start = esp_timer_get_time();
/*
#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
    enable_led(true);
    vTaskDelay(150 /
               portTICK_PERIOD_MS);  // The LED needs to be turned on ~150ms
                                     // before the call to esp_camera_fb_get()
    fb = esp_camera_fb_get();  // or it won't be visible in the frame. A better
                               // way to do this is needed.
    enable_led(false);
#else
    fb = esp_camera_fb_get();
#endif
*/

    fb = FeebeeCam::getImage();

    if (!fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        httpd_resp_send_500(req);
        FeebeeCam::_setup->applyToCamera();
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_set_hdr(req, "Content-Disposition",
                       "inline; filename=capture.jpg");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    char ts[32];
    snprintf(ts, 32, "%ld.%06ld", fb->timestamp.tv_sec, fb->timestamp.tv_usec);
    httpd_resp_set_hdr(req, "X-Timestamp", (const char *)ts);

    size_t fb_len = 0;
    if (fb->format == PIXFORMAT_JPEG) {
        fb_len = fb->len;
        res    = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    } else {
        jpg_chunking_t jchunk = {req, 0};
        res = frame2jpg_cb(fb, 80, jpg_encode_stream, &jchunk) ? ESP_OK
                                                                : ESP_FAIL;
        httpd_resp_send_chunk(req, NULL, 0);
        fb_len = jchunk.len;
    }
    esp_camera_fb_return(fb);
    FeebeeCam::_setup->applyToCamera();
    int64_t fr_end = esp_timer_get_time();
    ESP_LOGI(TAG, "JPG: %uB %ums", (uint32_t)(fb_len),
                (uint32_t)((fr_end - fr_start) / 1000));
    return res;
}

static esp_err_t stream_handler(httpd_req_t *req) {

    camera_fb_t *fb = NULL;
    struct timeval _timestamp;
    esp_err_t res       = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf   = NULL;
    char *part_buf[128];

    if (FeebeeCam::_setup)
        FeebeeCam::_setup->applyToCamera();

    static int64_t last_frame = 0;
    if (!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
    enable_led(true);
    isStreaming = true;
#endif

    FeebeeCam::light.turnOn();
    FeebeeCam::isCameraRunning = true;

    while (!FeebeeCam::stop) {

        fb = esp_camera_fb_get();
        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
        } else {
            _timestamp.tv_sec  = fb->timestamp.tv_sec;
            _timestamp.tv_usec = fb->timestamp.tv_usec;
            _jpg_buf_len = fb->len;
            _jpg_buf     = fb->buf;
        }

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY,
                                        strlen(_STREAM_BOUNDARY));
        }
        if (res == ESP_OK) {
            size_t hlen =
                snprintf((char *)part_buf, 128, _STREAM_PART, _jpg_buf_len,
                         _timestamp.tv_sec, _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf,
                                        _jpg_buf_len);
        }
        if (fb) {
            esp_camera_fb_return(fb);
            fb       = NULL;
            _jpg_buf = NULL;
        }
        
        if (_jpg_buf) {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }

        if (res != ESP_OK) {
            break;
        }

        ++FeebeeCam::frameCount;

        int64_t frameEndTime = esp_timer_get_time();
        int64_t frameTime = frameEndTime - FeebeeCam::lastTimeFramesCounted;

        if (frameTime > 0.0) {
            FeebeeCam::framesPerSecond =
                1000.00 * 1000.00 * (float)FeebeeCam::frameCount / (float)frameTime;
        }

        if (FeebeeCam::pause) {

            FeebeeCam::isCameraPaused = true;

            while (FeebeeCam::pause && !FeebeeCam::stop) {
                delay(1);
            }

            FeebeeCam::isCameraPaused = false;

        }

        FeebeeCam::resetCameraWatchDogTimer();
    }

    FeebeeCam::stop = false;
    FeebeeCam::isCameraPaused = false;
    FeebeeCam::pause = false;
    FeebeeCam::isCameraRunning = false;
    FeebeeCam::framesPerSecond = 0.0;

    FeebeeCam::light.turnOff();

#ifdef CONFIG_LED_ILLUMINATOR_ENABLED
    isStreaming = false;
    enable_led(false);
#endif

    last_frame = 0;
    return res;
}

static esp_err_t parse_get(httpd_req_t *req, char **obuf) {
    char *buf      = NULL;
    size_t buf_len = 0;

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = (char *)malloc(buf_len);
        if (!buf) {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            *obuf = buf;
            return ESP_OK;
        }
        free(buf);
    }
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

static int print_reg(char *p, sensor_t *s, uint16_t reg, uint32_t mask) {
    return sprintf(p, "\"0x%x\":%u,", reg, s->get_reg(s, reg, mask));
}

static esp_err_t status_get_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "application/json; charset-utf-8");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    FeebeeCam::status.assign();
    const std::string content = FeebeeCam::status.str();

    return httpd_resp_send(req, content.c_str(), content.length());
}

static int parse_get_var(char *buf, const char *key, int def) {
    char _int[16];
    if (httpd_query_key_value(buf, key, _int, sizeof(_int)) != ESP_OK) {
        return def;
    }
    return atoi(_int);
}

static BeeFishBScript::Variable getJSONFromPost(httpd_req_t *req) {

    BeeFishJSON::JSON json;
    BeeFishBScript::BScriptParser parser(json);
    size_t contentLength = req->content_len;

    size_t read = 0;
    size_t bufferLength = getPageSize();

    Data data = Data::create();
    esp_err_t err = ESP_OK;

    while (
            (read < contentLength) && 
            (err == ESP_OK) &&
            (parser.result() == BeeFishMisc::nullopt)
        ) 
    {
        if (contentLength - read < bufferLength)
            bufferLength = contentLength - read;

        ssize_t bytesRead = httpd_req_recv(req, (char*)data._readWrite, bufferLength);
        if (bytesRead > 0) {
            if (parser.result() == BeeFishMisc::nullopt) {
                parser.read(data, bufferLength);
                read += bufferLength;
            }
        }
    }

    if (parser.result() == true) {
        return parser.json();
    }
    else {
        err = httpd_resp_send_500(req);
        return undefined;
    }

}

static esp_err_t command_post_handler(httpd_req_t *req) {

    BeeFishBScript::ObjectPointer request = 
        (BeeFishBScript::ObjectPointer)getJSONFromPost(req);

    // Command
    BString command = (*request)["command"];

    BeeFishBScript::Object object;
    object["status"] = BeeFishBScript::Null();
    object["message"] = "Invalid command";

    bool _putToSleep = false;
    bool _downloadFiles = false;
    bool restart = false;

    if (command == "stop") {
        FeebeeCam::stopCamera();
        object["status"] = true;
        object["message"] = "Camera stopped";
    }
    else if (command == "reset") {
        FeebeeCam::_setup->reset();
        object["status"] = true;
        object["message"] = "Camera reset";
    }
    else if (command == "save") {
        FeebeeCam::_setup->save();
        object["status"] = true;
        object["message"] = "Camera saved";
    }
    else if (command == "sleep") {
        object["status"] = true;
        object["message"] = "Camera put to sleep";
        object["redirectURL"] = HOST "/beehive/";
        _putToSleep = true;
    }
    else if (command == "restart") {
        object["status"] = true;
        object["message"] = "Camera restarting";
        object["redirectURL"] = HOST "/beehive/";
        restart = true;
    }
    else if (command == "download") {
        object["status"] = true;
        object["message"] = "Downloading new firmware";
        object["statusURL"] = HOST "download";
        _downloadFiles = true;
    }

    std::stringstream stream;
    stream << object;
    std::string content = stream.str();

    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    esp_err_t err = httpd_resp_send(req, content.c_str(), content.length());

    cerr << "Sent camera command " << command << "..." << flush;

    if (_putToSleep) {
        FeebeeCam::commands.push(FeebeeCam::PUT_TO_SLEEP);
    }

    if (_downloadFiles) {
        FeebeeCam::commands.push(FeebeeCam::DOWNLOAD_FILES);
    }

    if (restart) {
        FeebeeCam::commands.push(FeebeeCam::RESTART);
    }

    cerr << "Ok" << endl;

    return ESP_OK;

}

static esp_err_t setup_json_post_handler(httpd_req_t *req) {

    BeeFishBScript::Variable json = getJSONFromPost(req);

    if (json == undefined)
        return ESP_FAIL;

    FeebeeCam::_setup->apply(json);
    FeebeeCam::_setup->load();
    FeebeeCam::_setup->_isSetup = true;

    if (!FeebeeCam::_setup->save()) {
        return httpd_resp_send_500(req);
    }

    BeeFishBScript::Object copy;
    FeebeeCam::_setup->assign(copy, false);

    std::stringstream stream;
    stream << copy;
    std::string content = stream.str();

    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    esp_err_t err = httpd_resp_send(req, content.c_str(), content.length());

    return ESP_OK;

}

static esp_err_t setup_json_get_handler(httpd_req_t *req) {

    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    std::stringstream stream;
    BeeFishBScript::Object reading;
    FeebeeCam::_setup->assign(reading, false);
    stream << reading;
    std::string string = stream.str();
    return httpd_resp_send(req, string.c_str(), string.length());
}

std::map<BeeFishBString::BString, BeeFishBString::BString> CONTENT_TYPES = {
    {"html", "text/html; charset=utf-8"},
    {"txt", "text/plain]; charset=utf-8"},
    {"js", "text/javascript; charset=utf-8"},
    {"json", "application/json; charset=utf-8"},
    {"jpg", "image/jpeg"},
    {"gif", "image/gif"},
    {"ico", "image/x-icon"}
};

std::map<BeeFishBString::BString, bool> CACHE_RULES = {
    {"html", false},
    {"txt", false},
    {"js", false},
    {"jpg", true},
    {"json", false},
    {"gif", true},
    {"ico", true}
};

static esp_err_t file_handler(httpd_req_t *req) {

    BString filename = req->uri;

    if ((filename.find('.') == BString::npos)  && !filename.endsWith("/"))
        filename += "/";

    if (filename.endsWith("/"))
        filename += "index.html";
    
    std::string _filename = filename.str();

    Serial.print("Getting ");
    Serial.print(_filename.c_str());
    Serial.print("...");
    
    esp_err_t err = ESP_OK;

    if (SPIFFS.exists(_filename.c_str())) {

        httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

        File file = SPIFFS.open(_filename.c_str(), "r");
        size_t size = file.size();

        vector<BString> parts = filename.split('.');
        const BString& extension = parts[parts.size() - 1];

        httpd_resp_set_type(req, CONTENT_TYPES[extension].str().c_str());
        std::stringstream stream;
        stream << size;
        std::string sizeString = stream.str();

        httpd_resp_set_hdr(req, "Content-Length", sizeString.c_str());

        bool cacheRule = CACHE_RULES[extension];

        if (cacheRule)
            httpd_resp_set_hdr(req, "Cache-Control", "max-age=31536000, immutable");
        else
            httpd_resp_set_hdr(req, "Cache-Control", "no-store, max-age=0");


        size_t chunkSize = getPageSize();
        size_t written = 0;
        Data data = Data::create();
        while (written < size) {
            if (written + chunkSize > size)
                chunkSize = size - written;
            file.read(data._readWrite, chunkSize);
            err = httpd_resp_send_chunk(req, (const char*)data._readWrite, chunkSize);
            if (err != ESP_OK) {
                return err;
            }
            written += chunkSize;
        }

        file.close();

        err = httpd_resp_send_chunk(req, NULL, 0);

        if (err == ESP_OK)
            std::cerr << " Ok" << std::endl;
        else
            std::cerr << " Fail" << std::endl;


        FeebeeCam::resetCameraWatchDogTimer();
        
    }
    else {
        std::cerr << " Not Found" << std::endl;

        if ( FeebeeCam::isConnectedToESPAccessPoint )
        {
            // Moved
            err = httpd_resp_set_status(req, "302 Moved Temporarily");

            httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

            // Redirect to setup
            const std::string location = (FeebeeCam::getURL() + "setup/index.html").str();
            cerr << "Redirecting to " << location << std::flush;

            err = httpd_resp_set_hdr(req, "Location", location.c_str());

            err = httpd_resp_send(req, NULL, 0);

            if (err == ESP_OK)
                cerr << " Ok";
            else
                cerr << " Fail";

            cerr << std::endl;

        }
        else {
            // Return 404 not found
            err = httpd_resp_send_404(req);
        }

    }

    return err;
}

static esp_err_t weather_handler(httpd_req_t *req) {

    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    std::stringstream stream;
    BeeFishBScript::Object reading = FeebeeCam::Weather::getWeather(false);
    stream << reading;
    std::string string = stream.str();
    return httpd_resp_send(req, string.c_str(), string.length());
}

static esp_err_t download_status_handler(httpd_req_t *req) {

    FeebeeCam::resetCameraWatchDogTimer();

    httpd_resp_set_type(req, "application/json; charset=utf-8");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    std::stringstream stream;
    stream << FeebeeCam::downloadStatus;
    std::string string = stream.str();
    return httpd_resp_send(req, string.c_str(), string.length());
}



namespace FeebeeCam {

    bool initializeWebServers() {

        FeebeeCam::deinitializeDNSServer();

        //if (FeebeeCam::isConnectedToESPAccessPoint)
            FeebeeCam::initializeDNSServer();

        if (camera_httpd) {
            std::cerr << "Stopping main web server" << std::flush;
            httpd_stop(camera_httpd);
            camera_httpd = nullptr;
            std::cerr << " Ok" << std::endl;
        }

        if (stream_httpd) {
            std::cerr << "Stopping stream web server" << std::flush;
            httpd_stop(stream_httpd);
            stream_httpd = nullptr;
            std::cerr << " Ok" << std::endl;
        }

        httpd_config_t config   = HTTPD_DEFAULT_CONFIG();
        config.task_priority = 2;
        config.max_uri_handlers = 16; 
        config.uri_match_fn = httpd_uri_match_wildcard;
        config.core_id = MAIN_WEB_SERVER_CORE;
        //config.max_open_sockets = MAX_OPEN_SOCKETS;

        httpd_uri_t weather_uri = {.uri  = "/weather",
                            .method   = HTTP_GET,
                            .handler  = weather_handler,
                            .user_ctx = NULL};

        httpd_uri_t status_get_uri = {.uri  = "/status",
                                .method   = HTTP_GET,
                                .handler  = status_get_handler,
                                .user_ctx = NULL};

        httpd_uri_t command_post_uri = {.uri  = "/command",
                                .method   = HTTP_POST,
                                .handler  = command_post_handler,
                                .user_ctx = NULL};

        httpd_uri_t capture_uri = {.uri      = "/capture",
                                .method   = HTTP_GET,
                                .handler  = capture_handler,
                                .user_ctx = NULL};

        httpd_uri_t stream_uri = {.uri      = "/",
                                .method   = HTTP_GET,
                                .handler  = stream_handler,
                                .user_ctx = NULL};
/*
        httpd_uri_t bmp_uri = {.uri      = "/bmp",
                            .method   = HTTP_GET,
                            .handler  = bmp_handler,
                            .user_ctx = NULL};
*/

        httpd_uri_t download_status_uri = {.uri = "/downloadStatus",
                            .method   = HTTP_GET,
                            .handler  = download_status_handler,
                            .user_ctx = NULL};

        httpd_uri_t setup_json_post_uri = {.uri = "/setup.json",
                            .method   = HTTP_POST,
                            .handler  = setup_json_post_handler,
                            .user_ctx = NULL};

        httpd_uri_t setup_json_get_uri = {.uri = "/setup.json",
                            .method   = HTTP_GET,
                            .handler  = setup_json_get_handler,
                            .user_ctx = NULL};

        httpd_uri_t file_uri = {.uri      = "/*",
                            .method   = HTTP_GET,
                            .handler  = file_handler,
                            .user_ctx = NULL};

        ra_filter_init(&ra_filter, 20);


        if (httpd_start(&camera_httpd, &config) == ESP_OK) {

            httpd_register_uri_handler(camera_httpd, &weather_uri);
            httpd_register_uri_handler(camera_httpd, &status_get_uri);
            httpd_register_uri_handler(camera_httpd, &command_post_uri);
            httpd_register_uri_handler(camera_httpd, &capture_uri);
            httpd_register_uri_handler(camera_httpd, &download_status_uri);
            httpd_register_uri_handler(camera_httpd, &setup_json_get_uri);
            httpd_register_uri_handler(camera_httpd, &setup_json_post_uri);
            httpd_register_uri_handler(camera_httpd, &file_uri);

        }
        else {
            std::cerr << "Error starting web server" << std::endl;
            return false;
        }

        std::cout << "Web Server started: "
                << FeebeeCam::getURL(config.server_port)
                << std::endl;

        config   = HTTPD_DEFAULT_CONFIG();
        config.server_port += 1;
        config.ctrl_port += 1;
        config.max_uri_handlers = 1;
        config.task_priority = 1;
        config.core_id = CAMERA_WEB_SERVER_CORE;
        config.max_open_sockets = 1;


        if (httpd_start(&stream_httpd, &config) == ESP_OK) {
            httpd_register_uri_handler(stream_httpd, &stream_uri);
            std::cout << "Camera stream server started: "
                    << FeebeeCam::getURL(config.server_port)
                    << std::endl;
        }
        else {
            std::cerr << "Error starting camera stream server" << std::endl;
            return false;
        }

        return true;
    }
}
