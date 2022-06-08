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

        static SSLConnection* _connection;

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

            if (!_connection || _connection->host() != _host || _connection->port() != _port) {
                if (_connection)
                    delete _connection;
                _connection = new SSLConnection(_host, _port);
            }

        }

        virtual ~WebRequest() {
            if (_webResponse)
                delete _webResponse;

            if (_parser)
                delete _parser;

        }

        virtual bool send() {

            try {

                BString url = "https://" + _host + _path + _query;
                cerr << url << endl;

                if (!_connection->secureConnection()) {
                    clog << "Opening connection" << endl;
                    _connection->open();
                }

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

                cerr << "Reading response" << endl;

                Data buffer = Data::create();

                while(_connection->secureConnection()) {
                    
                    while(millis() < timeout) {
                        
                        // read an incoming byte from the server and print it to serial monitor:
                        size_t length = _connection->read(buffer);


                        if (length > 0) {

                            _parser->read(buffer, length);

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

                            timeout = millis() + _timeout;
                        }
                    }

                    if (exit)
                        break;

                    if (millis() > timeout)
                    {
                        Serial.println("Timed out");
                        timedOut = true;
                        break;
                    }

                }

                flush();
                
                if ( _webResponse->headers()->result() == true && 
                    _webResponse->headers()->count("set-cookie") > 0 )
                {
                    cookie() = _webResponse->headers()->at("set-cookie");
                }

                return ( !timedOut && 
                        _parser->result() == true && 
                        statusCode() == 200 );
            }            
            catch(...) {
                _connection->close();
                cerr << "Error sending web request." << endl;
                return false;
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