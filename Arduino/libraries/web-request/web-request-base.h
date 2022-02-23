#ifndef FEEBEECAM_WEB_REQUEST
#define FEEBEECAM_WEB_REQUEST

#include <Arduino.h>
#include <map>
#include <functional>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <esp_log.h>
#include <esp_http_client.h>
#include <esp_tls.h>
#include <bee-fish.h>
#include "web-request-parser.h"

namespace FeebeeCam {

    class WebRequest {
    protected:
        int _statusCode = 0;
        BString _host;
        int _port = 443;
        BString _path;
        BString _query;

        BeeFishMisc::optional<BString> _body;
        BString _method;

        BeeFishWeb::WebResponse* _webResponse = nullptr;
        
        typedef std::function<void(const BeeFishBString::Data& data)> OnData;
        OnData _ondata = nullptr;
        
        static BeeFishMisc::optional<BString>& cookie() {
            static BeeFishMisc::optional<BString> cookie;
            return cookie;
        }

        WiFiClientSecure _client;
            
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
            _body(body),
            _webResponse(nullptr)
        {
            if (_body.hasValue())
                _method = "POST";
            else
                _method = "GET";
        }

        virtual ~WebRequest() {
            if (_webResponse)
                delete _webResponse;
        }

        virtual bool send() {
            
            initialize();

            FeebeeCam::WebRequestParser parser(*_webResponse);

            _client.setInsecure();

            if (!_client.connect(_host.c_str(), _port))
                return false;

            Serial.print("Connected to server ");
            Serial.println(_host.c_str());

            // make a HTTP request:
            // send HTTP header
            BString header =
                _method + " " + _path + _query + " HTTP/1.1";

            //Serial.println(header.c_str());
            
            _client.println(header.c_str());
            _client.print("Host: ");
            _client.println(_host.c_str());
            _client.println("Connection: close");
            if (cookie().hasValue()) {
                _client.print("Cookie: ");
                _client.println(cookie().value().c_str());
            }

            if (hasBody())
                _client.println("Content-Type: application/json");

            _client.println(); // end HTTP header

            if (hasBody()) {
                _client.println(_body.value());
            }

            bool exit = false;

            while(_client.connected()) {
                
                while(_client.available()) {
                    
                    // read an incoming byte from the server and print it to serial monitor:
                    Byte byte = _client.read();

                    //Serial.print((char)byte);

                    if ( !parser.match((char)byte) )
                    {
                        Serial.print("Exiting invalid match");
                        exit = true;
                        break;
                    }

                    if ( _webResponse->result() == false )
                    {
                        Serial.println("Exit failed parse");
                        exit = true;
                        break;
                    }

                    if ( _webResponse->result() == true )
                    {
                        Serial.println("Exit parsed Ok");
                        exit = true;
                        break;
                    }
                }

                if (exit)
                    break;
            }

            Serial.println();
            Serial.println("Disconnecting from _client");

            _client.stop();

            if (_webResponse->headers()->result() == true) {
                cookie() = _webResponse->headers()->at("set-cookie");
            }

            return (parser.result() == true);
        }

        bool hasBody() {
            return _body.hasValue();
        }
        
        virtual void setOnData(OnData ondata) {
            _ondata = ondata;
        }

        virtual void initialize() {
            if (_webResponse)
                delete _webResponse;
            _webResponse = new BeeFishWeb::WebResponse();
            _webResponse->setOnData(_ondata);

        }

        BeeFishWeb::WebResponse& webResponse() {
            return *_webResponse;
        }

        int statusCode() const {
            if (_webResponse && _webResponse->statusLine()->result() == true)
                return _webResponse->statusLine()->statusCode()->value();
            else
                return -1;
        }

        BeeFishMisc::optional<BString>& body() {
            return _body;
        }

    };

    
}

#endif