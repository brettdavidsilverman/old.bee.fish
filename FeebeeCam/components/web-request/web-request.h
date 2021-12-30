#ifndef FEEBEECAM_WEB_REQUEST
#define FEEBEECAM_WEB_REQUEST

#include <map>
#include <esp_log.h>
#include <esp_http_client.h>
#include <esp_tls.h>
#include <bee-fish.h>

#define TAG "FeebeeCamn WebRequest"

namespace FeebeeCam {

    class WebRequest {
    private:
        esp_http_client_handle_t _client = nullptr;
        int _statusCode;
        int _contentLength;
        BString _host;
        BString _path;
        BString _query;
        BString _body;
        BString _cookie;
        std::map<BString, BString> _responseHeaders;
    public:
        WebRequest(BString host, BString path = "/", BString query = "", BString body = "") :
            _host(host),
            _path(path),
            _query(query),
            _body(body)
        {

            esp_http_client_config_t config = {
                .host = _host,
                .path = _path,
                .query = _query,
                .method = HTTP_METHOD_GET,
                .event_handler = _http_event_handler,
                .transport_type = HTTP_TRANSPORT_OVER_SSL,
                .user_data = this
            };
    
            _client = esp_http_client_init(&config);
            esp_err_t err = ESP_OK;

            // Capture cookie response header
            _responseHeaders["set-cookie"] = "";
        }

        virtual void send() {

            // Set the request cookie header
            if (_cookie.size()) {
                esp_http_client_set_header(_client, "cookie", _cookie);
            }

            // POST
            if (_body.size()) {
                esp_http_client_set_method(_client, HTTP_METHOD_POST);
                esp_http_client_set_header(_client, "Content-Type", "application/json");
                esp_http_client_set_post_field(_client, _body, _body.size());
            }
            
            esp_err_t err = esp_http_client_perform(_client);
            if (err == ESP_OK) {
                _statusCode = esp_http_client_get_status_code(_client);
                _contentLength == esp_http_client_get_content_length(_client);
                ESP_LOGD(TAG, "HTTP POST Status = %d", _statusCode);
            } else {
                ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
            }

            _cookie = _responseHeaders["set-cookie"];

        }

        virtual void ondata(const char* buffer, size_t length) {
            const std::string str(buffer, length);
            cout << str;
        }

        virtual ~WebRequest() {
            if (_client)
                esp_http_client_cleanup(_client);
        }

        std::map<BString, BString>& responseHeaders() {
            return _responseHeaders;
        }

        virtual void captureHeader(BString header) {
            _responseHeaders[header] = "";
        }

        int statusCode() const {
            return _statusCode;
        }

        size_t contentLength() const {
            return _contentLength;
        }

        BString& body() {
            return _body;
        }

        static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
        {
            WebRequest* webRequest =  (WebRequest*)evt->user_data;
            
            switch(evt->event_id) {
                case HTTP_EVENT_ERROR:
                    ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
                    break;
                case HTTP_EVENT_ON_CONNECTED:
                    ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
                    break;
                case HTTP_EVENT_HEADER_SENT:
                    ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
                    break;
                case HTTP_EVENT_ON_HEADER: 
                    ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
                    {
                        std::map<BString, BString>& headers = webRequest->responseHeaders();
                        BString key = BString(evt->header_key).toLower();
                        if (headers.count(key) > 0)
                            headers[key] = evt->header_value;
                    }
                    break;
                case HTTP_EVENT_ON_DATA:
                    ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
                    webRequest->ondata((const char*)(evt->data), evt->data_len);
                    break;
                case HTTP_EVENT_ON_FINISH:
                    ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
                    break;
                case HTTP_EVENT_DISCONNECTED:
                    ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
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