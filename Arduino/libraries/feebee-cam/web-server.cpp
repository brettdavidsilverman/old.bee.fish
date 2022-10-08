#include <WiFi.h>
#include <esp_task_wdt.h>
#include "web-server.h"
#include "camera.h"
#include "light.h"
#include "file-server.h"
#include "weather.h"
#include "file-system.h"
#include "setup.h"


namespace FeebeeCam {

    WebServer* webServer = nullptr;
    WebServer* cameraWebServer = nullptr;
    

    // Example decleration
    //bool onWeather(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);

    bool initializeWebServer() {

        if (webServer)
            delete webServer;
                
        webServer = new WebServer(80, 1, 0);
        cameraWebServer = new WebServer(8080, 1, 1);

        webServer->paths()["/weather"]  = FeebeeCam::onWeather;
        webServer->paths()["/capture"]  = FeebeeCam::onCapture;
        webServer->paths()["/command"]  = FeebeeCam::onCommand;
        webServer->paths()["/settings"] = FeebeeCam::onSettings;
        webServer->paths()["/light"]    = FeebeeCam::onLight;
        webServer->paths()["/restart"]  = FeebeeCam::onRestart;
        webServer->paths()["/status"]   = FeebeeCam::onStatus;
        webServer->paths()["/download"] = FeebeeCam::onDownloadFiles;
        webServer->paths()["/camera"]   = FeebeeCam::onCamera;
        webServer->_defaultHandler      = FeebeeCam::onFileServer;

        //cameraWebServer->paths()["/camera"]   = FeebeeCam::onCamera;
        cameraWebServer->_defaultHandler      = FeebeeCam::onCamera;


        return true;

    }

    WebServer::WebServer(int port, int priority, int core) :
        _port(port),
        _priority(priority),
        _core(core)
    {
        _wifiServer = new WiFiServer(port);
        std::stringstream stream;
        stream << "WebServer:" << _port;
        _taskName = stream.str();
        _wifiServer->begin(_port);
    }

    WebServer::~WebServer() {

        delete _wifiServer;
    }

    void WebServer::loop() {


        if (_clientCount >= 1) {
            return;
        }

        WiFiClient wifiClient = _wifiServer->available();

        if (wifiClient) {
            TaskHandle_t handle = NULL;
            WebClient* webClient = new WebClient(*this, wifiClient);

            
            webClient->handleRequest();
            
            

            std::stringstream taskName;
            
            static size_t taskId = 0;
            taskName << "WebClient:" << taskId++;
            
            
            cerr << "Starting task " << taskName.str() << endl;

            xTaskCreatePinnedToCore(
                WebClient::handleClient,      // Task function. 
                taskName.str().c_str(),      // String with name of task. 
                12000,                // Stack size in bytes. 
                webClient,                 // Parameter passed as input of the task 
                _priority,                      // Priority of the task. 
                &handle,             // Task handle
                _core               // Pinned to core 
            );

 /*
            xTaskCreate(
                WebClient::handleClient,      // Task function. 
                taskName.str().c_str(),      // String with name of task. 
                12000,                // Stack size in bytes. 
                webClient,                 // Parameter passed as input of the task 
                _priority,                      // Priority of the task. 
                &handle             // Task handle
            );

*/
            if (handle == NULL) {
                cerr << "Couldn't start web server task " << taskName.str() << endl;
                delete webClient;
            }
            
        }

    }

    bool WebClient::readRequest() {

        char *inputBuffer = (char *)malloc(_pageSize);

        unsigned long timeOut = millis() + 40000;


        while (_wifiClient.connected() && _parser.result() == BeeFishMisc::nullopt)
        {
            size_t received;

            received = _wifiClient.read((uint8_t*)inputBuffer, _pageSize);

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

        bool result = _wifiClient.write(data, size) == size;
        
        return result;
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
            << "Content-Type: text/plain;charset=utf-8\r\n"
            << "Access-Control-Allow-Origin: *\r\n"
            << "Connection: keep-alive\r\n"
            << "\r\n";

         if (json->contains("flash"))
            light->toggleFlash();
         else
            light->toggle();

         BeeFishBScript::Object object {
            {"light", light->status()},
            {"flash", light->flashStatus()}
         };

         stream << object.str() << "\r\n";
         
         stream.flush();

         return true;
      }

      return false;
   };

}
