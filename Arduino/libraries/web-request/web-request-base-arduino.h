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
        int _port;
        BString _host;
        BString _path;
        BString _query;
        BeeFishMisc::optional<BString> _body;
        typedef std::map<BString, BeeFishMisc::optional<BString> > ResponseHeaders;
        ResponseHeaders _responseHeaders;
        
        typedef std::function<void(char character)> OnCharacter;
        OnCharacter _onChartacter = nullptr;

        static BeeFishMisc::optional<BString> _cookie;
        WiFiClientSecure _client;
        BeeFishParser::Parser _parser
        BeeFishWeb::WebResponse _response;
    public:
        WebRequest(
            BString host,
            BString path = "/",
            BString query = "",
            int     port = 443,
            BeeFishMisc::optional<BString> body = BeeFishMisc::nullopt
        ) :
            _host(host),
            _path(path),
            _query(query),
            _port(port),
            _body(body),
            _parser(_response)
        {
            // Capture cookie response header
            _responseHeaders["set-cookie"] = BeeFishMisc::nullopt;
            
        }

        virtual void send() {
            
            initialize();
            
            _client.setInsecure();

            BString url = _host + _path + _query;

            if(_client.connect(_host.c_str(), _port)) {
                INFO(TAG, "Connected to %s", _host.c_str());
            }
            else {
                ERROR(ESP_FAIL, TAG, "Error connecting to %s", _host.c_str());
            }

            BString method;

            if (_body.hasValue())
                method = "POST";
            else
                method = "GET";

            BString firstLine = method + " " + _path + " HTTP/1.1";
            BString hostLine = BString("Host: ") + _host;

            _client.println(firstLine.c_str());
            _client.println(hostLine);
            _client.println("Connection: close");

            Serial.println("Web Request Sending https request");

            // Set the request cookie header
            if (_cookie.hasValue()) {
                BString cookieHeader = "cookie: " + _cookie;
                _client.println(cookieHeader.c_str());
            }

            _client.println(); // end HTTP request header

            // POST
            if (_body.hasValue()) {
                Serial.println("Setting up POST request");
                std::string body;
                body = _body.value().str();
                _client.println(body.c_str());
            }
                        
            Serial.println("Sending request");
        
            while (_client.connected()) {
      
                while(_client.available()) {
                    // read an incoming char from the server and print them to serial monitor:
                    c = _client.read();
                    Serial.print(c);
                    if (!_parser.match(c)) {
                        Serial.print("{err}");
                        break;
                    }

                }

                if (parser.result() != BeeFishMisc::nullopt)
                    break;

                delay(10);
            }

            _cookie = _responseHeaders["set-cookie"];

        }

        bool hasBody() {
            return _body.hasValue();
        }
        
        virtual void setOnData(OnData ondata) {
            _ondata = ondata;
        }

        virtual void ondata(const char* buffer, size_t length) {
            INFO(TAG, "ondata");

            if (_ondata)
                _ondata(buffer, length);
            else {
                const std::string str(buffer, length);
                INFO(TAG, BString(str))
            }
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