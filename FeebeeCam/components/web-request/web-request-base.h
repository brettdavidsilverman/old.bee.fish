#ifndef FEEBEECAM_WEB_REQUEST
#define FEEBEECAM_WEB_REQUEST

#include <Arduino.h>
#include <map>
#include <functional>
#include <esp_log.h>
#include <esp_http_client.h>
#include <esp_tls.h>
#include "esp_crt_bundle.h"
#include <bee-fish.h>
#include <error.h>
#include <ssl-cert.h>

#define TAG "FeebeeCam WebRequest"

extern httpsserver::SSLCert * cert;

namespace FeebeeCam {

    class WebRequest {
    private:
        int _statusCode = 0;
        int _contentLength = 0;
        BString _host;
        BString _path;
        BString _query;
        BeeFishMisc::optional<BString> _body;
        typedef std::map<BString, BeeFishMisc::optional<BString> > ResponseHeaders;
        ResponseHeaders _responseHeaders;
        
        typedef std::function<void(const char* buffer, size_t length)> OnData;
        OnData _ondata = nullptr;

    protected:
        inline static BeeFishMisc::optional<BString> _cookie;

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

            BString url = _host + _path + _query;

            INFO(TAG, "URL: %s", url.c_str());
            esp_http_client_config_t config = {
                .url = url.c_str(),
                .event_handler = eventhandler,
                .user_data = this,
#ifdef LOCAL
                .skip_cert_common_name_check = true
#else
                .crt_bundle_attach = esp_crt_bundle_attach
#endif
             };

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
                _contentLength == esp_http_client_get_content_length(client);
                CHECK_ERROR(err, TAG, "HTTP POST Status = %d", _statusCode);
            } else {
                Serial.println("Error sending request");
                CHECK_ERROR(err, TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
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

        virtual void ondata(const char* buffer, size_t length) {
            Serial.println("WebRequest::ondata");

            if (_ondata)
                _ondata(buffer, length);
            else {
                const std::string str(buffer, length);
                cout << str;
            }
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

        size_t contentLength() const {
            return _contentLength;
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
                case HTTP_EVENT_HEADERS_SENT:
                    INFO(TAG, "HTTP_EVENT_HEADERS_SENT");
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
                    INFO(TAG, "HTTP_EVENT_ON_DATA");
                    webRequest->ondata((const char*)(evt->data), evt->data_len);
                    break;
                case HTTP_EVENT_ON_FINISH:
                    INFO(TAG, "HTTP_EVENT_ON_FINISH");
                    break;
                case HTTP_EVENT_DISCONNECTED:
                    INFO(TAG, "HTTP_EVENT_DISCONNECTED");
                    int mbedtls_err = 0;
                    esp_err_t err = esp_tls_get_and_clear_last_error(
                        (esp_tls_error_handle_t)(evt->data),
                        &mbedtls_err,
                        NULL
                    );
                    break;
            }
            return ESP_OK;
        }
    };

}

#endif