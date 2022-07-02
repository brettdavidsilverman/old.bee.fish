#ifndef WEB_CLIENT
#define WEB_CLIENT

#include <map>

#include "../b-string/string.h"
#include "../json/json-parser.h"
#include "../web-request/web-request.h"
#include "../b-script/b-script.h"

namespace BeeFishWebServer {

    class WebClient {
    
    protected:

    public:
        WebServer* _webServer;

        int _socket;

        const int _pageSize = getpagesize();

        int _statusCode = 200;

        BeeFishBString::BString _statusText = "OK";
        
        BeeFishBString::BString _contentType = "text/plain";

        BeeFishWeb::WebRequest _webRequest;

        BeeFishBScript::BScriptParser _parser;

        bool _terminateOnDelete = true;

        inline static const int PRIORITY = 1;
        
        WebClient(WebServer* webServer, int socket) :
            _webServer(webServer),
            _socket(socket),
            _webRequest(),
            _parser(_webRequest)
        {
        }

        virtual ~WebClient() {
            if (_socket > 0) {
                std::cerr << "Client socket closed:  " << _socket << std::endl;
                close(_socket);
                _socket = -1;
            }
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

        virtual bool readRequest() {

            char *inputBuffer = (char *)malloc(_pageSize);

            while (_parser.result() == BeeFishMisc::nullopt)
            {

                int received, fd, bytes_read;
                char *ptr;

                received = recv(_socket, inputBuffer, _pageSize, MSG_DONTWAIT);

                if (received < 0)
                {
                    // receive error
                    cerr << "web-server::respond::recv() error." << endl;
                    return false;
                }
                else if (received == 0)
                {
                    // receive socket closed
                    cerr << "web-server::respond::Client disconnected upexpectedly." << endl;
                    return false;
                }
                else
                {

                    const BeeFishBString::Data data(inputBuffer, received);

                    // message received
                    _parser.read(data);
                }
            }

            free(inputBuffer);

            return true;
        }

        virtual bool sendHeaders() {

            BeeFishBString::BStream output;

            // Prepare output buffer
            output._onbuffer = [this](const BeeFishBString::Data &data)
            {
                send(data.data(), data.size());
            };

            output << "HTTP/1.1 " << _statusCode << " " << _statusText << "\r\n"
                    "Server: " BEE_FISH_WEBSERVER_VERSION "\r\n"
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

        virtual bool send(const char* data, size_t size) {
            return (::send(_socket, data, size, 0) == size);
        }

        virtual bool send(const Byte* data, size_t size) {
            return (::send(_socket, data, size, 0) == size);
        }

        static void process(void* param) {

            WebClient* client = (WebClient*)param;

            std::cerr << "Client process on socket " << client->_socket << std::endl;


            if (!client->readRequest()) {
                delete client;
#ifndef SERVER
                vTaskDelete(NULL);
#endif            
                return;
            }

            if (client->_webRequest._firstLine->result() == true) {

                const BeeFishBString::BString& path = client->_webRequest.path();

                WebServer::Paths& paths = client->_webServer->paths();

                WebServer::OnPath func = nullptr;

                if (paths.count(path) > 0) {
                    cerr << "Matched Path " << path << endl;

                    func = paths.at(path);
                }
                else
                    func = client->_webServer->_defaultHandler;

                if (func) {

                    if (func(path, client))
                        cerr << "Path " << path << " successfully handled"  << endl;
                    else
                        cerr << "Path " << path << " failed" << endl;

                    delete client;
#ifndef SERVER
                    vTaskDelete(NULL);
#endif            
                    return;
                }
            }

            client->_statusCode = 404;
            client->_statusText = "Not Found";
            client->defaultResponse();
            delete client;
#ifndef SERVER
            vTaskDelete(NULL);
#endif            

        }

    };



}

#endif
