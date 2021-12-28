#pragma once
#include <esp_log.h>
#include <esp_http_client.h>
#include <bee-fish.h>

extern const char *TAG;

namespace FeebeeCam {

    class WebRequest {
    private:
        esp_http_client_handle_t _client = nullptr;
        int _statusCode;
        int _contentLength;

    public:
        WebRequest(BString host, BString path = "/", BString body = "", esp_http_client_method_t method = HTTP_METHOD_GET) {

            esp_http_client_config_t config = {
                .host = host,
                .path = path,
                .method = method,
                .transport_type = HTTP_TRANSPORT_OVER_SSL
            };
    
            _client = esp_http_client_init(&config);
            
            esp_err_t err = ESP_OK;

            if ((err = esp_http_client_open(_client, body.size())) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
                return;
            }

            if (body.size()) {

                std::string _body = body.str();

                int written = esp_http_client_write(_client, _body.c_str(), _body.size() );
                if (written != _body.size())
                {
                    ESP_LOGE(TAG, "Failed to write body");
                    return;
                }
            }

            _contentLength = esp_http_client_fetch_headers(_client);
            _statusCode = esp_http_client_get_status_code(_client);
            
            size_t totalReadLength = 0, readLength, length;
            size_t pageSize = getpagesize();
            char buffer[pageSize];

            while (totalReadLength < _contentLength) {
                length = pageSize;
                
                if (totalReadLength + length > _contentLength)
                    length = _contentLength - totalReadLength;

                readLength = esp_http_client_read(_client, buffer, length);

                if (readLength <= 0) {
                    ESP_LOGE(TAG, "Error read data");
                    break;
                }

                onRead(buffer, readLength);
                
                totalReadLength += readLength;
            }

            if (err == ESP_OK) {
                ESP_LOGI(TAG, "HTTPS Status = %d, contentLength = %d",
                    _statusCode,
                    _contentLength
                );
            } 
            else {
                ESP_LOGE(TAG, "Error perform http request %s", esp_err_to_name(err));
            }

            esp_http_client_close(_client);
        }

        virtual void onRead(const char* buffer, size_t length) {
            const std::string str(buffer, length);
            cout << str;
        }

        virtual ~WebRequest() {
            if (_client)
                esp_http_client_cleanup(_client);
        }

        int statusCode() const {
            return _statusCode;
        }

        size_t contentLength() const {
            return _contentLength;
        }
    };

}

