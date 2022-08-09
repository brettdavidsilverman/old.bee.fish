#ifndef FEEBEECAM_WEB_REQUEST
#define FEEBEECAM_WEB_REQUEST
#include <Arduino.h>
#include <map>
#include <functional>
#include <bee-fish.h>
#include "ssl-connection.h"
#include "setup.h"

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

        typedef std::function<void(const BeeFishBString::Data& data)> OnData;

        BeeFishBScript::BScriptParser* _parser = nullptr;
        BeeFishWeb::WebResponse* _webResponse = nullptr;
        static RTC_DATA_ATTR char _cookieData[512];


        int _timeout = 10000;
    public:
        static SSLConnection* _connection;

        OnData _ondata = nullptr;

        static BString getCookie() {
            return BString(_cookieData);
        }

        static void setCookie(BString& cookie) {
            if (cookie.size() >= 512)
            {
                std::cerr << "Cookie too large to eat" << std::endl;
                return;
            }
            memset(_cookieData, 0, 512);
            memcpy(_cookieData, cookie.c_str(), cookie.size());
        }

        static void setCookie(const char* cookie) {
            BString _cookie(cookie);
            setCookie(_cookie);
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

            if (!_connection) {
                _connection = new SSLConnection(_host, _port);
            }

            if (!_connection->connected())
                _connection->open();
                
            BString url = "https://" + _host + _path + _query;

            clog << "Sending http request: " << url << endl;

            // make a HTTP request:
            // send HTTP header
            BeeFishBString::BStream stream = _connection->getStream();

            BString header =
                _method + " " + _path + _query + " HTTP/1.1";

            stream << header << "\r\n";
            stream << "Host: " << _host << "\r\n";
            stream << "Connection: keep-alive" << "\r\n";
            BString cookie = getCookie();
            if (cookie.size()) {
                stream << "Cookie: " << cookie << "\r\n";
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

            Data buffer = Data::create();

            while(_connection->connected() && (millis() < timeout)) {
                
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
                BString cookie = _webResponse->headers()->at("set-cookie");
                setCookie(cookie);
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
                WebRequest::resetConnection();                
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
                return _webResponse->statusLine()->statusCode()->intValue();
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

        static void resetConnection() {
            if (_connection != nullptr)
                delete _connection;

            _connection = nullptr;
        }

    };

    
}

#endif