#ifndef FEEBEECAM_WEB_REQUEST
#define FEEBEECAM_WEB_REQUEST
#include <Arduino.h>
#include <map>
#include <functional>
#include <bee-fish.h>
#include "ssl-connection.h"

namespace FeebeeCam {

    class WebRequest {
    protected:
        int _statusCode = 0;
        
        BString _host;

        int _port = 443;

        BString _path;
        BString _query;

        BeeFishBScript::Variable _body;

        bool _hasBody = false;

        BString _method;

        typedef std::function<void(const BeeFishBString::Data& data)> OnData;

        BeeFishBScript::BScriptParser* _parser = nullptr;
        BeeFishWeb::WebResponse* _webResponse = nullptr;
        static RTC_DATA_ATTR char _cookieData[512];


        long int _timeout = WEB_REQUEST_TIMEOUT;
    public:
        static SSLConnection* _connection;

        OnData _ondata = nullptr;

        static BString getCookie() {
            return BString(_cookieData);
        }

        static void setCookie(BString& cookie) {

            std::string _cookie = cookie.str();
            
            if (_cookie.size() >= 512)
            {
                std::cerr << "Cookie too large to eat" << std::endl;
                return;
            }
            memset(_cookieData, 0, 512);
            memcpy(_cookieData, _cookie.c_str(), _cookie.size());
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

        virtual bool openConnection() {
            if (!_connection) {
                _connection = new SSLConnection(_host, _port);
            }

            if (!_connection->connected()) {
                _connection->open();
            }

            bool success = (_connection && _connection->connected());

            return success;
        }

        virtual bool send() {

            cout << "Sending http request to " << url() << endl;

            if (!openConnection())
                return false;

            // make a HTTP request:
            // send HTTP header
            BeeFishBString::BStream stream = _connection->getStream();

            if (!sendDefaultHeaders(stream))
                return false;

            if (hasBody()) {
                stream << "Content-Type: application/json; charset=utf-8" << "\r\n";
            }

            stream << "\r\n"; // end HTTP header
            
            if (hasBody()) {
                // Stream the body object to the _client
                std::string string = _body.str();
                stream.write(string.data(), string.size());
            }


            stream.flush();

            bool success = readResponse();

            if (success)
                cerr << "OK" << endl;
            else
                cerr << "Error WebRequest readResponse " << this->statusCode() << endl;

            return success;
        }

        virtual const BString& path() const {
            return _path;
        }

        virtual BString url() {
            return "https://" + _host + _path + _query;
        }

    protected:

        virtual bool sendDefaultHeaders(BStream& stream) {

            BString firstLine =
                _method + " " + _path + _query + " HTTP/1.1";

            stream << firstLine << "\r\n";
            stream << "Host: " << _host << "\r\n";
            stream << "Connection: keep-alive" << "\r\n";

            BString cookie = getCookie();
            if (cookie.size()) {
                stream << "Cookie: " << cookie << "\r\n";
            }
         
            return true;

        }

        virtual bool readResponse() {
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

            while ( _connection->connected() && (_timeout == -1 || millis() < timeout) ) {
                
                // read an incoming byte from the server and print it to serial monitor:
                size_t length = 0;

                length = _connection->read(buffer);

                if (length < 0) {
                    cerr << "Zero response from connection->read" << endl;
                    break;
                }

                if (length == 0) {
                    delay(10);
                    continue;
                }
#ifdef DEBUG
//                cerr.write((const char*)buffer._data, length);
#endif
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

            if (_timeout != -1 && millis() > timeout)
            {
                cerr << "Timed out" << endl;

                timedOut = true;
            }
            else {
                flush();
            }
            /*                
                // Reading till end of stream
                while (_connection->_client.available()) {
                    int c = _connection->_client.read();
                    //cerr << "{" << (char)c << "}" << std::flush;
                }
            }
*/
            if ( _webResponse->headers() &&
                _webResponse->headers()->result() == true && 
                _webResponse->headers()->contains("set-cookie") )
            {
                BString cookie = _webResponse->headers()->at("set-cookie");
                setCookie(cookie);
            }

            if ( _parser->result() == true && 
                statusCode() == 200 ) 
            {
                return true;
            }

            if ( timedOut ||
                _parser->result() != true ) 
            {
                WebRequest::reset();
                return false;
            }


            return false;

        }

    public:
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

        BeeFishBScript::Variable& body() {
            return _body;
        }

        BeeFishBScript::ObjectPointer responseBody() {
            
            BeeFishBScript::ObjectPointer object =
                (BeeFishBScript::ObjectPointer)_parser->json();
                
            return object;
        }

        void flush() {
            if (_webResponse)
                _webResponse->flush();
        }

        long int timeout() {
            return _timeout;
        }

        void setTimeout(long int timeout) {
            _timeout = timeout;
        }

        static void reset() {
            if (_connection != nullptr)
                delete _connection;

            _connection = nullptr;
        }

    };

    
}

#endif