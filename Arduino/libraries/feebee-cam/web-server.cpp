#include <WiFi.h>
#include <mutex>
#include <esp_task_wdt.h>
#include "web-server.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "file-system.h"
#include "setup.h"
#include "uploads.h"
#include "config.h"

namespace FeebeeCam {

    WebServer* webServer = nullptr;
    WebServer* webServerCamera = nullptr;

     // Example decleration
    //bool onWeather(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);

    bool initializeWebServers() {
        
        std::cerr << "Initializing web servers" << std::endl;
        
        if (webServer) {
            delete webServer;
            webServer = nullptr;
        }
                
        if (webServerCamera) {
            delete webServerCamera;
            webServerCamera = nullptr;
        }

        if (dnsServer) {
            dnsServer->stop();
            delete dnsServer;
            dnsServer = nullptr;
        }


        webServer       = new WebServer(80, 0/*2*/, MAIN_WEB_SERVER_CORE);
        webServerCamera = new WebServer(8080, 0 /*3*/, CAMERA_WEB_SERVER_CORE);

        webServer->paths()["/weather"]          = FeebeeCam::onWeather;
        webServer->paths()["/capture"]          = FeebeeCam::onCapture;
        webServer->paths()["/command"]          = FeebeeCam::onCommand;
        webServer->paths()["/setup.json"]       = FeebeeCam::onSetupBeehiveJSON;
        webServer->paths()["/light"]            = FeebeeCam::onLight;
        webServer->paths()["/downloadStatus"]   = FeebeeCam::onDownloadStatus;
        webServer->paths()["/status"]           = FeebeeCam::onStatus;

        if (FeebeeCam::isConnectedToESPAccessPoint)
        {
            std::cerr << "Setting up web server with redirect" << std::endl;
            webServer->_defaultHandler = FeebeeCam::onFileServerRedirect;
        }
        else {
            std::cerr << "Setting up web server with not found" << std::endl;
            webServer->_defaultHandler = FeebeeCam::onFileServer;

        }

        webServerCamera->_defaultHandler        = FeebeeCam::onCamera;

        webServer->start();
        webServerCamera->start();

        std::cerr << "Web servers initialized" << std::endl;
        std::cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << std::endl;

        return true;

    }

    WebServer::WebServer(int port, int priority, int core) :
        _port(port),
        _priority(priority),
        _core(core)
    {
        _server = new WiFiServer(port);
        std::stringstream stream;
        stream << "WebServer: " << _port;
        _taskName = stream.str();

    }


    WebServer::~WebServer() {

        if (_handle) {

            std::cerr << "Deleting web server task " << _taskName << std::flush;

            std::cerr << " Ending loop" << std::flush;

            _quit = true;

            while (_isRunning)
                delay(10);
                
            vTaskDelete(_handle);
            _handle = NULL;
            
            std::cerr << " Ok" << std::endl;
        }

        std::cerr << "Closing base server" << std::flush;

        _server->close();

        std::cerr << " Ok" << std::endl;

        std::cerr << "Deleting base server" << std::flush;
        delete _server;
        _server = nullptr;

        std::cerr << " Ok" << std::endl;



    }

    void WebServer::loop(void* param) {

        WebServer* webServer = (WebServer*)param;
        
        webServer->_isRunning = true;

        while (!webServer->_quit) {

            WiFiClient client = webServer->server()->available();

            if (client) {

                WebClient* webClient = new WebClient(*webServer, client);
                webClient->handleRequest();
                FeebeeCam::resetCameraWatchDogTimer();

            }

            delay(1);

        }

        webServer->_isRunning = false;

        while (1)
            ;

    }

    bool WebServer::start() {

        std::string taskName = _taskName.str();

        cerr << "Starting " << taskName << std::flush;

        uint32_t     stackSize = 10000;

        _quit = false;

        if (_core == -1) {
            xTaskCreate(
                WebServer::loop,   // Task function. 
                _taskName.str().c_str(),           // String with name of task. 
                stackSize,                       // Stack size in bytes. 
                this,                  // Parameter passed as input of the task 
                _priority,                          // Priority of the task. 
                &_handle                     // Task handle
            );
        }
        else {
            xTaskCreatePinnedToCore(
                WebServer::loop,   // Task function. 
                _taskName.str().c_str(),           // String with name of task. 
                stackSize,                       // Stack size in bytes. 
                this,                  // Parameter passed as input of the task 
                _priority,                          // Priority of the task. 
                &_handle,                    // Task handle
                _core                           // Pinned to core 
            );
        }

        if (_handle) {

            server()->begin(_port);

            cerr << " OK" << endl;
            
            return true;
        }
        else {
            cerr << " Error" << endl;
            return false;
        }

        return true;

    }



}