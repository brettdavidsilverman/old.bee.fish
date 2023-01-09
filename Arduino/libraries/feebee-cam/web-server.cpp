#include <WiFi.h>
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

        FeebeeCam::socketLock = false;

        if (webServer)
            delete webServer;
                
        if (webServerCamera)
            delete webServerCamera;

        webServer       = new WebServer(80, 2, MAIN_WEB_SERVER_CORE);
        webServerCamera = new WebServer(8080, 3, CAMERA_WEB_SERVER_CORE);

        webServer->paths()["/weather"]          = FeebeeCam::onWeather;
        webServer->paths()["/capture"]          = FeebeeCam::onCapture;
        webServer->paths()["/command"]          = FeebeeCam::onCommand;
        webServer->paths()["/setup.json"]       = FeebeeCam::onSetupBeehive;
        webServer->paths()["/light"]            = FeebeeCam::onLight;
        webServer->paths()["/downloadStatus"]   = FeebeeCam::onDownloadStatus;
        webServer->paths()["/status"]           = FeebeeCam::onStatus;
        webServer->paths()["/redirect"]         = FeebeeCam::onRedirect;
        //webServer->paths()["/connecttest.txt"]  = FeebeeCam::onRedirect;
        webServer->paths()["/generate_204"]     = FeebeeCam::onGenerate204;
        webServer->paths()["/fwlink"]           = FeebeeCam::onRedirect;

        webServer->_defaultHandler              = FeebeeCam::onFileServer;

        webServerCamera->_defaultHandler        = FeebeeCam::onCamera;

        webServer->start();
        webServerCamera->start();

        std::cerr << "Web servers initialized" << std::endl;

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

        delete _server;
    }

    void WebServer::loop(void* param) {

        WebServer* webServer = (WebServer*)param;

        while (1) {

            delay(1);

            if (FeebeeCam::socketLock)
                continue;

            WiFiClient client = webServer->server()->available();

            if (client) {
                
                WebClient* webClient = new WebClient(*webServer, client);
                webClient->handleRequest();
            }


        }

    }

    bool WebServer::start() {

        std::string taskName = _taskName.str();

        cerr << "Starting " << taskName << std::flush;

        TaskHandle_t handle    = nullptr;
        uint32_t     stackSize = 6000;

        if (_core == -1) {
            xTaskCreate(
                WebServer::loop,   // Task function. 
                _taskName.str().c_str(),           // String with name of task. 
                stackSize,                       // Stack size in bytes. 
                this,                  // Parameter passed as input of the task 
                _priority,                          // Priority of the task. 
                &handle                     // Task handle
            );
        }
        else {
            xTaskCreatePinnedToCore(
                WebServer::loop,   // Task function. 
                _taskName.str().c_str(),           // String with name of task. 
                stackSize,                       // Stack size in bytes. 
                this,                  // Parameter passed as input of the task 
                _priority,                          // Priority of the task. 
                &handle,                    // Task handle
                _core                           // Pinned to core 
            );
        }

        if (handle) {

            server()->begin(_port);

            cerr << " OK" << endl;
        }
        else {
            cerr << " Error" << endl;
        }

        return true;

    }



}