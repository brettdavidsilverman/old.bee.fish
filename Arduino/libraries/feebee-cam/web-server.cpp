#include "web-server2.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "setup.h"
#include "web-server-defines.h"

namespace FeebeeCam {

    WebServer* webServer80;
    WebServer* webServer8080;
    
    // Example decleration
    //bool onWeather(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);

    bool initializeWebServer() {

        webServer80 = new WebServer(80, 1);
        webServer8080 = new WebServer(8080, 0);

        webServer80->paths()["/weather"]          = FeebeeCam::onWeather;
        webServer8080->paths()["/camera"]           = FeebeeCam::onCamera;
        webServer80->paths()["/capture"]          = FeebeeCam::onCapture;
        webServer80->paths()["/command"]          = FeebeeCam::onCommand;
        webServer80->paths()["/settings"]         = FeebeeCam::onSettings;
        webServer80->paths()["/light"]            = FeebeeCam::onLight;
        
        webServer80->_defaultHandler              = FeebeeCam::onFileServer;

        webServer80->start();
        webServer8080->start();

        //webServer->paths()["/light"]          = FeebeeCam::onLight;
        //webServer->paths()["/setup/settings"] = FeebeeCam::onSetupSettings;
        //webServer->paths()["/weather"]        = FeebeeCam::onWeather;

        return true;

    }

    WebServer::WebServer(int port, int core) : _port(port), _core(core) {
        _server = new WiFiServer(port);
        std::stringstream stream;
        stream << "WebServer:" << _port;
        _taskName = stream.str();
    }

    WebServer::~WebServer() {
        delete _server;
    }

    void WebServer::loop(void* param) {

        WebServer* webServer = (WebServer*)param;

        webServer->server()->begin(webServer->_port);


        cerr << "Server started on port " << webServer->_port << endl;

        for (;;)
        {

            WiFiClient client = webServer->server()->available();

            if (client) {
                WebClient webClient(*webServer, &client);
                webClient.handleRequest();
            }

            delay(10);
        }

    }

    bool WebServer::start() {

        clog << "Starting " << _taskName << endl;
        
        TaskHandle_t xHandle = NULL;

        xTaskCreatePinnedToCore(
            WebServer::loop,      // Task function. 
            _taskName.c_str(),      // String with name of task. 
            5120, //2048,                // Stack size in bytes. 
            this,                 // Parameter passed as input of the task 
            WebServer::PRIORITY,     // Priority of the task. 
            &xHandle,             // Task handle
            _core                  // Pinned to core 
        );

        if (xHandle == NULL)
            cerr << "Error starting " << _taskName << endl;

        return xHandle != NULL;

    }

    WebClient::~WebClient() {
        _client->stop();
    }
    
    bool WebClient::readRequest() {

        char *inputBuffer = (char *)malloc(_pageSize);

        unsigned long timeOut = millis() + 20000;

        while (_client->connected() && _parser.result() == BeeFishMisc::nullopt)
        {

            size_t received = _client->read((uint8_t*)inputBuffer, _pageSize);

            const BeeFishBString::Data data(inputBuffer, received);

            // message received
            _parser.read(data);

            if (millis() > timeOut) {
                cerr << "Receive timed out" << endl;
                return false;
            }

            timeOut = millis() + 20000;
        }

        free(inputBuffer);

        return true;
    }

    bool WebClient::send(const char* data, size_t size) {
        return (_client->write(data, size) == size);
    }

    bool WebClient::send(const Byte* data, size_t size) {
        return (_client->write(data, size) == size);
    }


}
