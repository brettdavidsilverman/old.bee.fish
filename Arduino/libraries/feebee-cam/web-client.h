#ifndef WEB_CLIENT__H
#define WEB_CLIENT__H

#include <map>
#include <lwip/sockets.h>
#include <bee-fish.h>
#include "web-server-base.h"
#include "commands.h"
#include "web-request.h"
#include "camera.h"
#include "wifi.h"

class WiFiClient;

namespace FeebeeCam {


    class WebClient {
    public:
        WebServer& _webServer;
        WiFiClient _client;

        const int _pageSize = getPageSize();

        int _statusCode = 200;

        BeeFishBString::BString _statusText = "OK";
        
        BeeFishBString::BString _contentType = "text/plain";
        signed long long _contentLength = -1;
        BeeFishBString::BString _cacheControl = "no-store, max-age=0";

        BeeFishWeb::WebRequest _webRequest;

        BeeFishBScript::BScriptParser _parser;

        bool _terminateOnDelete = true;

        //inline static const int PRIORITY = 1;
        
        BeeFishBString::BStream _output;
        BeeFishBString::BStream _chunkedOutput;
        bool _error;

        bool _chunkedEncoding = false;
        bool _buffersSetup = false;

        WebClient(WebServer& webServer, WiFiClient& client) :
            _webServer(webServer),
            _client(client),
            _webRequest(),
            _parser(_webRequest)
        {
            _error = false;
        }

        virtual ~WebClient() {
            _client.stop();
        }

        virtual void setupBuffers() {

            if (_buffersSetup)
                return;

            // Prepare output buffore for chunke4d encoding
            _output._onbuffer = [this](const BeeFishBString::Data &data)
            {
                ssize_t sent = 0;

                if (!_error && _client.connected())
                    sent = send(data._data, data.size());

                if (sent != data.size() && !_error) {
                    cerr << "Error sending from onbuffer {" << sent << ", " << data.size() << "}" << endl;
                    _error = true;
                }

#ifdef DEBUG
//                cerr.write((const char*)data._data, data.size());
#endif
                delay(1);
            };

            // Prepare output buffer for chunked encoding
            _chunkedOutput._onbuffer = [this](const BeeFishBString::Data &data)
            {
                if (!sendChunk(data)) {
                    cerr << "Error sending chunk from chunked on buffer" << endl;
                    _error = true;
                }
               
            };

            _buffersSetup = true;

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
            
            setupBuffers();
            return _output;

        }

        BeeFishBString::BStream& getChunkedOutputStream() {
            _chunkedEncoding = true;
            setupBuffers();
            return _chunkedOutput;
        }


        void handleRequest() {

            if (FeebeeCam::isCameraRunning) {
                FeebeeCam::pauseCamera();
            }


            WebClient* client = this;

            client->setupBuffers();

            if (client->readRequest()) {

                if (client->_webRequest.firstLine().result() == true) {

                    const BeeFishBString::BString& path = client->_webRequest.path();

                    WebServer::Paths& paths = client->_webServer.paths();

                    WebServer::OnPath handler = nullptr;

                    if (paths.count(path) > 0)
                        handler = paths.at(path);
                    else
                        handler = client->_webServer._defaultHandler;

                    if (handler) {
                        
                        if (!handler(path, client)) {
                            
                            cerr << "ERROR WITH PATH: " << path << endl;
                        }

                    }
                    else {
                        client->_statusCode = 404;
                        client->_statusText = "Not Found";
                        client->defaultResponse();
                    }

                    client->flush();

                }
            }
            else {
                cerr << "Error with parsing from WebClient" << endl;
            }

            if (client->_error) {
                FeebeeCam::commands.push(FeebeeCam::INITIALIZE_WEBSERVER);
                //RESTART_AFTER_ERROR();
            }

            delete client;
            //vTaskDelete(NULL);

            if (FeebeeCam::isCameraPaused) {
                FeebeeCam::resumeCamera();
            }

        }

        virtual bool readRequest() {

            Data data = Data::create();

            unsigned long timeOut = millis() + WEB_REQUEST_TIMEOUT;


            while (_client.connected() && _parser.result() == BeeFishMisc::nullopt)
            {
                size_t received = 0;

                if (_client.available()) {
                    received = _client.read((uint8_t*)data._readWrite, data.size());
                }

                if (received > 0) {
                    // message received
                    _parser.read(data, received);
                }

                if (millis() > timeOut) {
                    cerr << "Receive timed out" << endl;
                    return false;
                }

                timeOut = millis() + WEB_REQUEST_TIMEOUT;

                delay(1);
            }

            return (_parser.result() == true);
        }

        virtual bool readFinalBytes() {

            size_t i = 0;

            while (_client.available()) {
                _client.read();
                ++i;
            }

            if (i!= 0)
                std::cerr << i << " bytes trailing" << std::endl;

            return true;

        }

        virtual bool sendHeaders() {

            readFinalBytes();

            _error = false;

            BString origin;
            origin = FeebeeCam::getURL(8080);

            std::cerr << "Using access-control-origin: " << origin << std::endl;

            _output << 
                "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n"
                "server: FeebeeCam Server" <<  "\r\n";

            if (_contentType.length()) 
                _output <<  "content-type: " << _contentType << "\r\n";
            
            if (_contentLength >= 0)
                _output << "content-length: " << (size_t)_contentLength << "\r\n";
            else if (_chunkedEncoding)
                _output << "transfer-encoding: chunked\r\n";


            if (_cacheControl.length()) {
                _output << "cache-control: " << _cacheControl << "\r\n";
            }

            _output <<
                "connection: keep-alive\r\n" <<
//                "keep-alive: max=2\r\n" <<
//                "connection: close\r\n" <<
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

            flush();

            return !_error;
        }

        virtual void flush() {
            _output.flush();
            _client.flush();
        }

        virtual BeeFishBScript::Variable& body() {
            return _parser.value();
        }


        virtual ssize_t send(const char* data, ssize_t size) {
            return send((Byte*)data, size);
        }

        virtual ssize_t send(const Byte* data, ssize_t size) {
//#warning "hack"
            static std::mutex lock;
            std::lock_guard<std::mutex> guard(lock);

            ssize_t written = _client.write(data, size);

            return written;
/*
            std::cerr << "Sending " << size << std::flush;


            int socketFileDescriptor = _client.fd();
            ssize_t sent = ::send(socketFileDescriptor, (void*)data, size, MSG_WAITALL);

            std::cerr << " Sent " << sent << std::endl;

            return sent;

            std::cerr << "Sending: " << size << std::flush;
            int socketFileDescriptor = _client.fd();
            ssize_t totalSent = 0;
            unsigned long timeOut = WEB_REQUEST_TIMEOUT + millis();
            while ( (totalSent < size) && 
                    (millis() < timeOut) )
            {

                ssize_t sent = ::send(socketFileDescriptor, (void*) (data + totalSent), size - totalSent, MSG_DONTWAIT);

                if (sent > 0)
                    totalSent += sent;
                    
                delay(1);
            }

            if (totalSent == size)
                std::cerr << " Ok" << std::endl;
            else
                std::cerr << " Fail: " << totalSent << std::endl;

            return totalSent;
*/
/*
            size_t written = 0;
            if (_client.connected()) {
                
                written = _client.write(data, size);
                
            }
            return written;
*/
        }
    };



}

#endif