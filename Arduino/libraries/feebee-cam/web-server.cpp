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

    WebServer* webServer80 = nullptr;
    WebServer* webServer8080 = nullptr;

    // Example decleration
    //bool onWeather(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);

    bool initializeWebServer() {

        if (webServer80)
            delete webServer80;
                
        if (webServer8080)
            delete webServer8080;

        webServer80 = new WebServer(80, 2);
        webServer8080 = new WebServer(8080, 2);

        webServer80->paths()["/weather"]          = FeebeeCam::onWeather;
        webServer80->paths()["/capture"]          = FeebeeCam::onCapture;
        webServer80->paths()["/command"]          = FeebeeCam::onCommand;
        webServer80->paths()["/settings"]         = FeebeeCam::onSettings;
        webServer80->paths()["/setup.json"]       = FeebeeCam::onSetup_JSON;
        webServer80->paths()["/light"]            = FeebeeCam::onLight;
        webServer80->paths()["/restart"]          = FeebeeCam::onRestart;
        webServer80->paths()["/download"]         = FeebeeCam::onDownloadFiles;
        webServer80->paths()["/status"]           = FeebeeCam::onStatus;

        webServer80->_defaultHandler              = FeebeeCam::onFileServer;

        webServer8080->paths()["/camera"]         = FeebeeCam::onCamera;

        webServer80->start();
        webServer8080->start();

        return true;

    }

    WebServer::WebServer(int port, int priority) :
        _port(port),
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

//        webServer->server()->begin(webServer->_port);

        static int webClientId = 0;

        //for (;;)
        {
            
            //while (WebClient::_count >= MAX_WEB_CLIENTS)
            //    delay(10);

            WiFiClient client = webServer->server()->available();

            if (client) {
                WebClient* webClient = new WebClient(*webServer, client);
                
                //WebClient::handleRequest(webClient);
                //continue;

                TaskHandle_t handle = nullptr;
                std::stringstream stream;
                stream << "WebClient " << ++webClientId;
                std::string taskName = stream.str();

                xTaskCreate(//PinnedToCore(
                    WebClient::handleRequest,   // Task function. 
                    taskName.c_str(),           // String with name of task. 
                    5000,                      // Stack size in bytes. 
                    webClient,                  // Parameter passed as input of the task 
                    2,                          // Priority of the task. 
                    &handle//,                    // Task handle
                    //1                           // Pinned to core 
                );

                if (handle == nullptr) {
                    cerr << "Couldnt create web client task" << endl;
                    delete webClient;
                }
            }

          //  delay(10);

        }

    }

    bool WebServer::start() {

        std::string taskName = _taskName.str();

        server()->begin(_port);

        return true;

        xTaskCreatePinnedToCore(
            WebServer::loop,      // Task function. 
            taskName.c_str(),      // String with name of task. 
            2048,                // Stack size in bytes. 
            this,                 // Parameter passed as input of the task 
            1,                    // Priority of the task. 
            &_xHandle,             // Task handle
            1               // Pinned to core 
        );

        if (_xHandle == NULL)
            cerr << "Error starting " << _taskName << endl;

        return _xHandle != NULL;

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
            << "     body: JSON.stringify(body)"
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

         const BeeFishBScript::ObjectPointer json = client->_parser.value();

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