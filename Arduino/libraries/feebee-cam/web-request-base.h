#ifndef FEEBEECAM_WEB_REQUEST
#define FEEBEECAM_WEB_REQUEST

#include <Arduino.h>
#include <map>
#include <functional>
#include <WiFi.h>
#include "ssl-connection.h"
#include <bee-fish.h>

namespace FeebeeCam {

    class WebRequest {
    protected:
        int _statusCode = 0;
        
        BString _host;

        int _port = 443;

        SSLConnection* _connection = nullptr;

        BString _path;
        BString _query;

        BeeFishBScript::Object _body;
        bool _hasBody = false;

        BString _method;

        typedef std::function<void(const BeeFishBString::Data& data)> OnData;

        BeeFishBScript::BScriptParser* _parser = nullptr;
        BeeFishWeb::WebResponse* _webResponse = nullptr;

        int _timeout = 20000;

    public:
        OnData _ondata = nullptr;

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

            _connection = new SSLConnection(_host, _port);

            clog << "Opening connection" << endl;
            _connection->open();

        }

        virtual ~WebRequest() {
            if (_webResponse)
                delete _webResponse;

            if (_parser)
                delete _parser;

            if (_connection)
                delete _connection;
        }

        virtual bool send() {

            if (!_connection || !_connection->secureConnection())
                return false;
                
            BString url = "https://" + _host + _path + _query;
            cerr << url << endl;


            clog << "Sending http request..." << endl;

            // make a HTTP request:
            // send HTTP header
            BeeFishBString::BStream stream = _connection->getStream();

            BString header =
                _method + " " + _path + _query + " HTTP/1.1";

            stream << header << "\r\n";
            stream << "Host: " << _host << "\r\n";
            stream << "Connection: keep-alive" << "\r\n";
            if (cookie().hasValue()) {
                stream << "Cookie: " << cookie().value() << "\r\n";
            }

            if (hasBody())
                stream << "Content-Type: application/json" << "\r\n";

            stream << "\r\n"; // end HTTP header

            if (hasBody()) {
                // Stream the body object to the _client
                stream << _body;
            }

            stream.flush();

            if (_webResponse)
                delete _webResponse;

            if (_parser)
                delete _parser;

            _webResponse = new BeeFishWeb::WebResponse;
            _parser = new BeeFishBScript::BScriptParser(*_webResponse);

            _webResponse->setOnData(_ondata);

            unsigned long timeout = millis() + _timeout;
            bool timedOut = false;

            cerr << "Reading response" << endl;

            Data buffer = Data::create();

            while(_connection->secureConnection() && (millis() < timeout)) {
                
                // read an incoming byte from the server and print it to serial monitor:
                size_t length = _connection->read(buffer);

                if (length < 0) {
                    cerr << "Zero response from connection->read" << endl;
                    break;
                }

                if (length == 0) {
                    delay(10);
                    continue;
                }

                _parser->read(buffer, length);

                if ( _webResponse->result() == false )
                {
                    Serial.println("Exit failed parse");
                    break;
                }

                if ( _webResponse->result() == true )
                {
                    break;
                }

                timeout = millis() + _timeout;
            }

            if (millis() > timeout)
            {
                cerr << "Timed out" << endl;
                timedOut = true;
            }

            flush();
            
            if ( _webResponse->headers()->result() == true && 
                _webResponse->headers()->count("set-cookie") > 0 )
            {
                cookie() = _webResponse->headers()->at("set-cookie");
            }

            if ( !timedOut && 
                _parser->result() == true && 
                statusCode() == 200 ) 
            {
                return true;
            }

            if ( timedOut ||
                _parser->result() != true ) 
            {
                delete _connection;
                _connection = nullptr;
            }


            return false;

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
            if (_webResponse && _webResponse->statusLine()->result() == true)
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