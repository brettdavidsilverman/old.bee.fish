#ifndef FEEBEECAM_WEB_REQUEST
#define FEEBEECAM_WEB_REQUEST

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE

#include <Arduino.h>
#include <map>
#include <functional>
#include <esp_log.h>
#include <esp_http_client.h>
#include <esp_tls.h>
#include <bee-fish.h>

#define TAG "WebRequest"

namespace FeebeeCam {

    class WebRequest {
    protected:
        int _statusCode = 0;
        BString _host;
        BString _path;
        BString _query;
        BeeFishMisc::optional<BString> _body;
        typedef std::map<BString, BeeFishMisc::optional<BString> > ResponseHeaders;
        ResponseHeaders _responseHeaders;
        
        typedef std::function<void(const BeeFishBString::Data& data)> OnData;
        OnData _ondata = nullptr;

        static BeeFishMisc::optional<BString> _cookie;

    public:
        WebRequest(
            BString host,
            BString path = "/",
            BString query = "",
            BeeFishMisc::optional<BString> body = BeeFishMisc::nullopt
        ) :
            _host(host),
            _path(path),
            _query(query),
            _body(body)
        {
            // Capture cookie response header
            _responseHeaders["set-cookie"] = BeeFishMisc::nullopt;
        }

        virtual void send() {
            
            initialize();

            BString url = _host + _path + _query;
            Serial.println(url.c_str());

            INFO(TAG, "Web Request URL: %s", url.c_str());
            
            Serial.println("Creating client config");
            esp_http_client_config_t config;
            memset(&config, 0, sizeof(esp_http_client_config_t));

            config.url = url.c_str();
            config.event_handler = eventhandler;
            config.user_data = this;
            config.skip_cert_common_name_check = true;            

            Serial.println("Initializing client");
            esp_http_client_handle_t client = esp_http_client_init(&config);
            esp_err_t err = ESP_OK;


            Serial.println("Web Request Sending https request");

            // Set the request cookie header
            if (_cookie.hasValue()) {
                esp_http_client_set_header(client, "cookie", _cookie.value());
            }

            // POST
            std::string body;
            if (_body.hasValue()) {
                Serial.println("Setting up POST request");
                body = _body.value().str();
                esp_http_client_set_method(client, HTTP_METHOD_POST);
                esp_http_client_set_header(client, "Content-Type", "application/json");
                esp_http_client_set_post_field(client, body.c_str(), body.size());
            }
                        
            Serial.println("Sending request");
            err = esp_http_client_perform(client);

            if (err == ESP_OK) {
                Serial.println("Getting result");
                _statusCode = esp_http_client_get_status_code(client);
//                _contentLength == esp_http_client_get_content_length(client);
                CHECK_ERROR(err, TAG, "HTTP POST Status = %d", _statusCode);
            } else {
                Serial.println("Error sending request");
                CHECK_ERROR(err, TAG, "HTTP POST failed");
            }

            _cookie = _responseHeaders["set-cookie"];

            if (client)
                esp_http_client_cleanup(client);

        }

        bool hasBody() {
            return _body.hasValue();
        }
        
        virtual void setOnData(OnData ondata) {
            _ondata = ondata;
        }

        virtual void ondata(const BeeFishBString::Data& data) {
            INFO(TAG, "ondata");

            if (_ondata)
                _ondata(data);
        }

        virtual void initialize() {

        }

        virtual ~WebRequest() {
        }

        std::map<BString, BeeFishMisc::optional<BString>>& responseHeaders() {
            return _responseHeaders;
        }

        virtual void captureHeader(BString header) {
            _responseHeaders[header] = BeeFishMisc::nullopt;
        }

        int statusCode() const {
            return _statusCode;
        }

        BeeFishMisc::optional<BString>& body() {
            return _body;
        }

        static BeeFishMisc::optional<BString>& cookie() {
            return _cookie;
        }

        static esp_err_t eventhandler(esp_http_client_event_t *evt)
        {
            WebRequest* webRequest =  (WebRequest*)evt->user_data;
            
            switch(evt->event_id) {
                case HTTP_EVENT_ERROR:
                    INFO(TAG, "HTTP_EVENT_ERROR");
                    break;
                case HTTP_EVENT_ON_CONNECTED:
                    INFO(TAG, "HTTP_EVENT_ON_CONNECTED");
                    break;
                case HTTP_EVENT_ON_HEADER: 
                    INFO(TAG, "HTTP_EVENT_ON_HEADER");
                    {
                        ResponseHeaders& headers = 
                            webRequest->responseHeaders();
                        BString key = BString(evt->header_key).toLower();
                        if (headers.count(key) > 0)
                            headers[key] = evt->header_value;
                    }
                    break;
                case HTTP_EVENT_ON_DATA: 
                    {
                        INFO(TAG, "HTTP_EVENT_ON_DATA");
                        const BeeFishBString::Data data(evt->data, evt->data_len);
                        webRequest->ondata(data);
                    }
                    break;
                case HTTP_EVENT_ON_FINISH:
                    INFO(TAG, "HTTP_EVENT_ON_FINISH");
                    break;
                case HTTP_EVENT_DISCONNECTED:
                    INFO(TAG, "HTTP_EVENT_DISCONNECTED");
                    break;
            }
            return ESP_OK;
        }
    };

}

#endif