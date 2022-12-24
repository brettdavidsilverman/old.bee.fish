#include <WiFi.h>
#include <esp_task_wdt.h>
#include "web-server.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "file-system.h"
#include "setup.h"
#include "config.h"

namespace FeebeeCam {

    WebServer* webServer = nullptr;
    WebServer* webServerCamera = nullptr;

     // Example decleration
    //bool onWeather(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);

    bool initializeWebServers() {

        std::cerr << "Initializing web servers" << std::endl;

        if (webServer)
            delete webServer;
                
        if (webServerCamera)
            delete webServerCamera;

        webServer       = new WebServer(80, 2, WEB_SERVER_CORE);
        webServerCamera = new WebServer(8080, 3, CAMERA_CORE);

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
        stream << "WebServer:" << _port;
        _taskName = stream.str();
    }

    WebServer::~WebServer() {

        delete _server;
    }

    void WebServer::loop(void* param) {

        if (WebClient::_count > 2)
            return;

        WebServer* webServer = (WebServer*)param;

        static int webClientId = 0;


        WiFiClient client = webServer->server()->available();

        if (client) {
            WebClient* webClient = new WebClient(*webServer, client);
            
            //WebClient::handleRequest(webClient);
            //continue;

            TaskHandle_t handle = nullptr;
            std::stringstream stream;
            stream << "WebClient " << ++webClientId;
            std::string taskName = stream.str();
            
            BaseType_t  core      = webServer->_core;
            UBaseType_t priority  = webServer->_priority;
            uint32_t    stackSize = 6000; //4000;

            if (core == -1) {
                xTaskCreate(
                    WebClient::handleRequest,   // Task function. 
                    taskName.c_str(),           // String with name of task. 
                    stackSize,                       // Stack size in bytes. 
                    webClient,                  // Parameter passed as input of the task 
                    priority,                          // Priority of the task. 
                    &handle                     // Task handle
                );
            }
            else {
                xTaskCreatePinnedToCore(
                    WebClient::handleRequest,   // Task function. 
                    taskName.c_str(),           // String with name of task. 
                    stackSize,                       // Stack size in bytes. 
                    webClient,                  // Parameter passed as input of the task 
                    priority,                          // Priority of the task. 
                    &handle,                    // Task handle
                    core                           // Pinned to core 
                );
            }

            if (handle == nullptr) {
                cerr << "Couldnt create web client task" << endl;
                delete webClient;
            }
        }

    }

    bool WebServer::start() {

        std::string taskName = _taskName.str();

        cerr << "Starting " << taskName << std::flush;

        server()->begin(_port);

        cerr << " OK" << endl;

        return true;

    }



}