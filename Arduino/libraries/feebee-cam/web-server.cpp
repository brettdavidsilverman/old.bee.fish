#include <WiFi.h>
#include <esp_task_wdt.h>
#include "web-server.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "setup.h"
#include "file-system.h"


namespace FeebeeCam {

    WebServer* webServer80 = nullptr;
    WebServer* webServer8080 = nullptr;

    // Example decleration
    //bool onWeather(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);

    bool initializeWebServer() {

        if (webServer80)
            delete webServer80;
                
        if (webServer8080)
            delete webServer8080;

        webServer80 = new WebServer(80, 1, 2);
        webServer8080 = new WebServer(8080, 0, 3);

        webServer80->paths()["/weather"]          = FeebeeCam::onWeather;
        webServer80->paths()["/capture"]          = FeebeeCam::onCapture;
        webServer80->paths()["/command"]          = FeebeeCam::onCommand;
        webServer80->paths()["/settings"]         = FeebeeCam::onSettings;
        webServer80->paths()["/light"]            = FeebeeCam::onLight;
        webServer80->paths()["/restart"]          = FeebeeCam::onRestart;
        webServer80->paths()["/download"]         = FeebeeCam::onDownloadFiles;

        webServer80->_defaultHandler              = FeebeeCam::onFileServer;

        webServer8080->paths()["/camera"]         = FeebeeCam::onCamera;

        webServer80->start();
        webServer8080->start();

        //webServer->paths()["/light"]          = FeebeeCam::onLight;
        //webServer->paths()["/setup/settings"] = FeebeeCam::onSetupSettings;
        //webServer->paths()["/weather"]        = FeebeeCam::onWeather;

        return true;

    }

    WebServer::WebServer(int port, int core, int priority) :
        _port(port),
        _core(core),
        _priority(priority)
    {
        _server = new WiFiServer(port);
        std::stringstream stream;
        stream << "WebServer:" << _port;
        _taskName = stream.str();
    }

    WebServer::~WebServer() {

        if (_xHandle)
            vTaskDelete(_xHandle);

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
                client.stop();
            }

            delay(100);
        }

    }

    bool WebServer::start() {

        clog << "Starting " << _taskName << endl;
        

        xTaskCreatePinnedToCore(
            WebServer::loop,      // Task function. 
            _taskName.c_str(),      // String with name of task. 
            8192,                // Stack size in bytes. 
            this,                 // Parameter passed as input of the task 
            _priority,     // Priority of the task. 
            &_xHandle,             // Task handle
            _core                  // Pinned to core 
        );

        if (_xHandle == NULL)
            cerr << "Error starting " << _taskName << endl;

        return _xHandle != NULL;

    }

    bool WebClient::readRequest() {

        char *inputBuffer = (char *)malloc(_pageSize);

        unsigned long timeOut = millis() + 40000;


        while (_client->connected() && _parser.result() == BeeFishMisc::nullopt)
        {
            size_t received;

            received = _client->read((uint8_t*)inputBuffer, _pageSize);

            const BeeFishBString::Data data(inputBuffer, received);

            // message received
            _parser.read(data);

            if (millis() > timeOut) {
                cerr << "Receive timed out" << endl;
                return false;
            }

            timeOut = millis() + 40000;
        }

        free(inputBuffer);

        return true;
    }

    bool WebClient::send(const char* data, size_t size) {
        return send((Byte*)data, size);
    }

    bool WebClient::send(const Byte* data, size_t size) {

        bool result = _client->write(data, size) == size;
        
        return result;
    }


}
