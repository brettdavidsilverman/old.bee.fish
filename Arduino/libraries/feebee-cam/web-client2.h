#ifndef WEB_CLIENT2__H
#define WEB_CLIENT2__H

#include <map>
#include <bee-fish.h>
#include "commands.h"

class WiFiClient;

namespace FeebeeCam {

    void resetConnection();
    
    class WebClient {
    
    protected:

    public:
        WebServer& _webServer;
        WiFiClient* _client;

        const int _pageSize = getpagesize();

        int _statusCode = 200;

        BeeFishBString::BString _statusText = "OK";
        
        BeeFishBString::BString _contentType = "text/plain";

        BeeFishWeb::WebRequest _webRequest;

        BeeFishBScript::BScriptParser _parser;

        bool _terminateOnDelete = true;

        inline static const int PRIORITY = 1;
        
        WebClient(WebServer& webServer, WiFiClient* client) :
            _webServer(webServer),
            _client(client),
            _webRequest(),
            _parser(_webRequest)
        {
        }

        virtual bool defaultResponse() {

            std::cerr << "WebClient::defaultResponse::sendHeaders" << std::endl;

            if (!sendHeaders())
                return false;


            BeeFishBString::BStream output = getChunkedOutputStream();

            std::cerr << "WebClient::defaultResponse::sendBody" << std::endl;
            if (!sendBody(output)) 
                return false;

            if (!sendChunk())
                return false;

            std::cerr << "WebClient::~defaultResponse::return true" << std::endl;
            return true;

        }

        BeeFishBString::BStream getChunkedOutputStream() {
            
            BeeFishBString::BStream output;

            // Prepare output buffore for chunke4d encoding
            output._onbuffer = [this](const BeeFishBString::Data &data)
            {
                sendChunk(data);
            };

            return output;

        }

        BeeFishBString::BStream getOutputStream() {
            
            BeeFishBString::BStream output;

            // Prepare output buffore for chunke4d encoding
            output._onbuffer = [this](const BeeFishBString::Data &data)
            {
                send(data.data(), data.size());
            };

            return output;

        }

        virtual bool handleRequest() {
            
            if (!readRequest()) {
                return false;
            }

            if (_webRequest._firstLine && _webRequest._firstLine->result() == true) {

                const BeeFishBString::BString& path = _webRequest.path();

                WebServer::Paths& paths = _webServer.paths();

                WebServer::OnPath func = nullptr;

                if (paths.count(path) > 0) {
                    cerr << "Matched Path " << path << endl;

                    func = paths.at(path);
                }
                else
                    func = _webServer._defaultHandler;

                if (func) {
                    
                    bool funcResult;

                    if (funcResult = func(path, this))
                        cerr << "Path " << path << " successfully handled"  << endl;
                    else {
                        cerr << "Path " << path << " failed" << endl;
                        
                        FeebeeCam::resetConnection();
                    }

                    return funcResult;
                }
            }

            _statusCode = 404;
            _statusText = "Not Found";
            defaultResponse();

            return true;

        }

        virtual bool readRequest();

        virtual bool sendHeaders() {

            BeeFishBString::BStream output;

            // Prepare output buffer
            output._onbuffer = [this](const BeeFishBString::Data &data)
            {
                send(data.data(), data.size());
            };

            output << "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n"
                    "Server: esp32/FeebeeCam server" <<  "\r\n"
                    "Content-Type: " << _contentType << "\r\n"
                    "Connection: keep-alive\r\n"
                    "Transfer-Encoding: chunked\r\n"
                    "\r\n";

            output.flush();

            return true;
        }

        virtual bool sendBody(BeeFishBString::BStream& output) {

            if (_parser.result() == true)
            {
                output << "Parsed valid request";
            }
            else
            {
                output << "Invalid request";
            }

            output.flush();

            return true;
        }

        virtual bool sendChunk(const BeeFishBString::Data& data) {

            std::stringstream stream;
            stream << std::hex << data.size();
            std::string size = stream.str();

            if (!send(size.c_str(), size.size()))
                return false;

            if (!send("\r\n", 2))
                return false;

            if (!send(data.data(), data.size()))
                return false;

            if (!send("\r\n", 2))
                return false;

            return true;
        }

        virtual bool sendChunk() {
            BeeFishBString::Data data;
            return sendChunk(data);
        }

        virtual bool send(const char* data, size_t size);

        virtual bool send(const Byte* data, size_t size);

    };



}

#endif