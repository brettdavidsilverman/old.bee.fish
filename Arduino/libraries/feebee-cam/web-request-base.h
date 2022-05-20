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
        
        static BString _connectedHost;
        static int _connectedPort;

        BString _host;
        int _port = 443;

        BString _path;
        BString _query;

        BeeFishBScript::Object _body;
        bool _hasBody = false;

        BString _method;

        typedef std::function<void(const BeeFishBString::Data& data)> OnData;
        OnData _ondata = nullptr;

        BeeFishBScript::BScriptParser* _parser = nullptr;
        BeeFishWeb::WebResponse* _webResponse = nullptr;

        int _timeout = 10000;

        static WiFiClientSecure* _client;

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
            _hasBody(hasBody)
        {
            if (_hasBody)
                _method = "POST";
            else
                _method = "GET";

        }

        virtual ~WebRequest() {
            if (_webResponse)
                delete _webResponse;

            if (_parser)
                delete _parser;
        }

        virtual bool send() {
            
            if (    _host != _connectedHost || 
                    _port != _connectedPort || 
                    !_client )  
            {
                if (_client) {
                    _client->stop();
                    delete _client;
                }
                _client = new WiFiClientSecure();
                _client->setInsecure();
            }

            if (!_client->connected()) {
                cerr << "Connecting to host " << _host << endl;
                if (!_client->connect(_host.c_str(), _port))
                    return false;
                _connectedHost = _host;
                _connectedPort = _port;
            }


            BString url = "https://" + _host + _path + _query;
            Serial.println(url.c_str());

            // make a HTTP request:
            // send HTTP header
            BString header =
                _method + " " + _path + _query + " HTTP/1.1";

            //Serial.println(header.c_str());
            
            _client->println(header.c_str());
            _client->print("Host: ");
            _client->println(_host.c_str());
            _client->println("Connection: keep-alive");
            if (cookie().hasValue()) {
                _client->print("Cookie: ");
                _client->println(cookie().value().c_str());
            }

            if (hasBody())
                _client->println("Content-Type: application/json");

            _client->println(); // end HTTP header

            if (hasBody()) {

                // Stream the body object to the _client
                BeeFishBString::BStream stream;

                stream.setOnBuffer(
                    [this](const Data& buffer) {
                        _client->write(buffer.data(), buffer.size());
                    }
                );

                stream << _body;

                stream.flush();
            }

            bool exit = false;

            if (_webResponse)
                delete _webResponse;

            if (_parser)
                delete _parser;

            _webResponse = new BeeFishWeb::WebResponse;
            _parser = new BeeFishBScript::BScriptParser(*_webResponse);

            _webResponse->setOnData(_ondata);

            unsigned long timeout = millis() + _timeout;
            bool timedOut = false;

            while(_client->connected()) {
                
                if (millis() > timeout)
                {
                    Serial.println("Timed out");
                    timedOut = true;
                    break;
                }

                while(_client->available()) {
                    
                    // read an incoming byte from the server and print it to serial monitor:
                    Byte byte = _client->read();

                    //cerr << (char)byte;

                    if ( !_parser->match((char)byte) )
                    {
                        Serial.println("Exiting invalid match");
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

                delay(10);
            }

            flush();
            
            if ( _webResponse->headers()->result() == true && 
                 _webResponse->headers()->count("set-cookie") > 0 )
            {
                cookie() = _webResponse->headers()->at("set-cookie");
            }

            if (timedOut || _parser->result() != true) {
                // If error with parsing, reset the client
                _client->stop();
                delete _client;
                _client = nullptr;
            }

            return ( !timedOut && 
                     _parser->result() == true && 
                     statusCode() == 200 );
        }

        bool hasBody() {
            return _hasBody;
        }
        
        virtual void setOnData(OnData ondata) {
            _ondata = ondata;
        }

        BeeFishWeb::WebResponse& webResponse() {
            return *_webResponse;
        }

        int statusCode() const {
            if (_webResponse->statusLine()->result() == true)
                return _webResponse->statusLine()->statusCode()->value();
            else
                return -1;
        }

        BeeFishBScript::Object& body() {
            return _body;
        }

        BeeFishBScript::ObjectPointer responseBody() {
            
            BeeFishBScript::ObjectPointer object =
                (BeeFishBScript::ObjectPointer)_parser->value();
                
            return object;
        }

        void flush() {
            if (_webResponse)
                _webResponse->flush();
        }

        int timeout() {
            return _timeout;
        }

        void setTimeout(int timeout) {
            _timeout = timeout;
        }


    };

    
}

#endif