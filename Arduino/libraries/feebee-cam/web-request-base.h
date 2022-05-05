#ifndef FEEBEECAM_WEB_REQUEST
#define FEEBEECAM_WEB_REQUEST

#include <Arduino.h>
#include <map>
#include <functional>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <bee-fish.h>

namespace FeebeeCam {

    class WebRequest {
    protected:
        int _statusCode = 0;
        BString _host;
        int _port = 443;
        BString _path;
        BString _query;

        BeeFishBScript::Object _body;
        bool _hasBody = false;

        BString _method;

        BeeFishWeb::WebResponse* _webResponse = nullptr;
        
        typedef std::function<void(const BeeFishBString::Data& data)> OnData;
        OnData _ondata = nullptr;
        

    public:

        static BeeFishMisc::optional<BString>& cookie() {
            static BeeFishMisc::optional<BString> cookie;
            return cookie;
        }

        WebRequest(
            BString host,
            BString path = "/",
            BString query = "",
            bool hasBody = false
        ) :
            _host(host),
            _path(path),
            _query(query),
            _hasBody(hasBody),
            _webResponse(nullptr)
        {
            if (_hasBody)
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

            WiFiClientSecure client;
            
            client.setInsecure();

            if (!client.connect(_host.c_str(), _port))
                return false;

            BString url = "https://" + _host + _path + _query;
            Serial.println(url.c_str());

            // make a HTTP request:
            // send HTTP header
            BString header =
                _method + " " + _path + _query + " HTTP/1.1";

            //Serial.println(header.c_str());
            
            client.println(header.c_str());
            client.print("Host: ");
            client.println(_host.c_str());
            client.println("Connection: close");
            if (cookie().hasValue()) {
                client.print("Cookie: ");
                client.println(cookie().value().c_str());
            }

            if (hasBody())
                client.println("Content-Type: application/json");

            client.println(); // end HTTP header

            if (hasBody()) {

                // Stream the body object to the client
                BeeFishBString::BStream stream;

                stream.setOnBuffer(
                    [&client](const Data& buffer) {
                        client.write(buffer.data(), buffer.size());
                    }
                );

                stream << _body;

                stream.flush();
            }

            bool exit = false;

            BeeFishBScript::BScriptParser parser(*_webResponse);

            while(client.connected()) {
                
                while(client.available()) {
                    
                    // read an incoming byte from the server and print it to serial monitor:
                    Byte byte = client.read();

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
                        exit = true;
                        break;
                    }
                }

                if (exit)
                    break;
            }

            client.stop();

            if (_webResponse->headers()->result() == true) {
                if (_webResponse->headers()->count("set-cookie") > 0)
                    cookie() = _webResponse->headers()->at("set-cookie");
            }

            return (parser.result() == true && statusCode() == 200);
        }

        bool hasBody() {
            return _hasBody;
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

        BeeFishBScript::Object& body() {
            return _body;
        }

        void flush() {
            _webResponse->flush();
        }

    };

    
}

#endif