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

     // Example decleration
    //bool onWeather(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);

    bool initializeWebServer() {

        if (webServer)
            delete webServer;
                
        webServer = new WebServer(80, 2, -1);

        webServer->paths()["/weather"]          = FeebeeCam::onWeather;
        webServer->paths()["/capture"]          = FeebeeCam::onCapture;
        webServer->paths()["/command"]          = FeebeeCam::onCommand;
        webServer->paths()["/setup.json"]       = FeebeeCam::onSetupBeehive;
        webServer->paths()["/light"]            = FeebeeCam::onLight;
        webServer->paths()["/downloadStatus"]   = FeebeeCam::onDownloadStatus;
        webServer->paths()["/status"]           = FeebeeCam::onStatus;
        webServer->paths()["/camera"]           = FeebeeCam::onCamera;

        webServer->_defaultHandler              = FeebeeCam::onFileServer;


        webServer->start();

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
            uint32_t    stackSize = 4000;

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


    bool onLight(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

      BeeFishBString::BStream& stream = client->getOutputStream();

      if (client->_webRequest.method() == "GET") {
         
         stream
            << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: text/html;charset=utf-8\r\n"
            << "Access-Control-Allow-Origin: *\r\n"
            << "Connection: keep-alive\r\n"
            << "\r\n"
            << "<html>"
            <<    "<head>"
            <<       "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=1\"/>"
            <<    "</head>"
            <<    "<body>"
            <<       "<button onclick='toggleLight(false)'>Toggle Light</button>"
            <<       "<button onclick='toggleLight(true)'>Toggle Flash</button>"
            <<          "<br />"
            <<          "<div id=\"response\"></div>"
            <<          "<script>"
            << "function toggleLight(flash) {"
            << "  var body;"
            << "  if (flash)"
            << "     body = {\"flash\": \"toggle\"};"
            << "  else"
            << "     body = {\"light\": \"toggle\"};"
            << "  var params = {"
            << "     method: \"POST\","
            << "     body: JSON.stringify(body),"
            << "     headers: "
            << "        new Headers(["
            << "           [\"Content-Type\", \"application/json; charset=utf-8\"]"
            << "        ])"
            << "  };"
            << "  fetch('/light', params).then("
            << "     function(response) {"
            << "        return response.text();"
            << "     }"
            << "  ).then("
            << "     function(text) {"
            << "        document.getElementById('response').innerHTML = text;"
            << "     }"
            << "  ).catch("
            << "     function(error) {"
            << "        document.getElementById('response').innerHTML = error;"
            << "        toggleLight();"
            << "     }"
            << "  );"
            << "}"
            <<      "</script>"
            <<   "</body>"
            << "</html>\r\n";

         stream.flush();

         light->turnOn();

         return true;
      }

      else if (client->_webRequest.method() == "POST") {

         const BeeFishBScript::ObjectPointer json = client->_parser.json();

         stream
            << "HTTP/1.1 200 OK\r\n"
            << "content-type: application/json; charset=utf-8\r\n"
            << "access-control-allow-origin: *\r\n"
            << "connection: keep-alive\r\n"
            << "\r\n";

         if (json->contains("flash"))
            light->toggleFlash();
         else
            light->toggle();

         BeeFishBScript::Object object {
            {"light", light->status()},
            {"flash", light->flashStatus()}
         };

         stream << object;
         
         stream.flush();

         return true;
      }

      return false;
   };

}