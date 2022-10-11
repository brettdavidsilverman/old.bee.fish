#ifndef WEB_CLIENT__H
#define WEB_CLIENT__H

#include <map>
#include <bee-fish.h>
#include "commands.h"

class WiFiClient;

namespace FeebeeCam {


    class WebClient {
    
    protected:

    public:
        WebServer& _webServer;
        WiFiClient* _client;

        const int _pageSize = getPageSize();

        int _statusCode = 200;

        BeeFishBString::BString _statusText = "OK";
        
        BeeFishBString::BString _contentType = "text/plain";
        size_t _contentLength = 0;

        BeeFishWeb::WebRequest _webRequest;

        BeeFishBScript::BScriptParser _parser;

        bool _terminateOnDelete = true;

        //inline static const int PRIORITY = 1;
        
        BeeFishBString::BStream _output;
        BeeFishBString::BStream _chunkedOutput;
        bool _error;

        WebClient(WebServer& webServer, WiFiClient* client) :
            _webServer(webServer),
            _client(client),
            _webRequest(),
            _parser(_webRequest)
        {
            _error = false;

            // Prepare output buffore for chunke4d encoding
            _output._onbuffer = [this](const BeeFishBString::Data &data)
            {
                if (!send(data._data, data.size()))
                    _error = true;
            };

            // Prepare output buffer for chunked encoding
            _chunkedOutput._onbuffer = [this](const BeeFishBString::Data &data)
            {
                if (!sendChunk(data))
                    _error = true;
            };

        }

        virtual bool defaultResponse() {

            std::cerr << "WebClient::defaultResponse::sendHeaders" << std::endl;

            if (!sendHeaders())
                return false;


            if (!sendBody()) 
                return false;

            if (!sendFinalChunk())
                return false;

            return !_error;

        }

        BeeFishBString::BStream& getOutputStream() {
            
            return _output;

        }

        BeeFishBString::BStream& getChunkedOutputStream() {
            
            return _chunkedOutput;
        }


        virtual bool handleRequest() {
            
            if (!readRequest()) {
                return false;
            }

            if (_webRequest._firstLine && _webRequest._firstLine->result() == true) {

                const BeeFishBString::BString& path = _webRequest.path();

                WebServer::Paths& paths = _webServer.paths();

                WebServer::OnPath func = nullptr;

                if (paths.count(path) > 0)
                    func = paths.at(path);
                else
                    func = _webServer._defaultHandler;

                if (func) {
                    
                    if (!func(path, this)) {
                        
                        FeebeeCam::restartAfterError();

                        return false;
                    }

                    return true;
                }
            }

            _statusCode = 404;
            _statusText = "Not Found";
            defaultResponse();

            return true;

        }

        virtual bool readRequest();

        virtual bool readFinalBytes() {

            size_t i = 0;

            while (_client->available()) {
                _client->read();
                ++i;
            }

            if (i!= 0)
                std::cerr << i << " bytes trailing" << std::endl;

            return true;

        }

        virtual bool sendHeaders() {

            readFinalBytes();

            BString origin;
            if (_webRequest.headers().contains("origin")) {
                origin = _webRequest.headers()["origin"];
            }
            else
                origin = "*";

            _output << 
                "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n"
                "server: esp32/FeebeeCam server" <<  "\r\n" <<
                "content-type: " << _contentType << "\r\n";
            
            if (_contentLength > 0)
                _output << "content-length: " << _contentLength << "\r\n";

            _output <<
                "connection: keep-alive\r\n" <<
                "transfer-encoding: chunked\r\n" <<
                "access-control-allow-origin: " << origin << "\r\n" <<
                "\r\n";

            return !_error;
        }

        virtual bool sendBody() {

            if (_parser.result() == true)
            {
                _output << "Parsed valid request";
            }
            else
            {
                _output << "Invalid request";
            }

            return !_error;
        }

        virtual bool sendChunk(const BeeFishBString::Data& data) {

            _output << std::hex << data.size()
                    << "\r\n";

            _output.write((const char*)data._data, data.size());

            _output << "\r\n";

            return !_error;
        }

        virtual bool sendFinalChunk() {

            _chunkedOutput.flush();

            _output << "0"
                    << "\r\n"
                    << "\r\n";

            _output.flush();

            _client->flush();

            return !_error;
        }

        virtual BeeFishBScript::Variable& body() {
            return _parser.value();
        }

        virtual bool send(const char* data, size_t size);

        virtual bool send(const Byte* data, size_t size);

    };



}

#endif