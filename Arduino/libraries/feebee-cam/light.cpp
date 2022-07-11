#include <pimoroni-light.h>

namespace FeebeeCam {   

//   PimoroniLight customLight;
   Light* light = nullptr;

   bool initializeLight() {

      if (initializeMultiplexer()) {
         light = new Light();
      }
      else
         light = new PimoroniLight();

      if (light->initialize()) {
         light->turnOff();
         return true;
      }
      else
         return false;

   }    

   bool onLight(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client) {

      BeeFishBString::BStream stream = client->getOutputStream();

      if (client->_webRequest.method() == "GET") {
         
         stream
            << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: text/html;charset=utf-8\r\n"
            << "Access-Control-Allow-Origin: null\r\n"
            << "Connection: keep-alive\r\n"
            << "\r\n"
            << "<html>"
            <<    "<head>"
            <<       "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=1\"/>"
            <<    "</head>"
            <<    "<body>"
            <<       "<button onclick='toggleLight()'>Toggle Light</button>"
            <<          "<br />"
            <<          "<div id=\"response\"></div>"
            <<          "<script>"
            << "function toggleLight() {"
            << "  var body = {\"light\": \"toggle\"};"
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

         client->_webRequest.value();
         const BeeFishJSON::Object& json = client->_webRequest.json();

         stream
            << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: text/plain;charset=utf-8\r\n"
            << "Access-Control-Allow-Origin: null\r\n"
            << "Connection: keep-alive\r\n"
            << "\r\n";

         if (json.result() == true) {

            light->toggle();

            BeeFishBScript::Object object {
               {"status", light->status()}
            };

            stream << object << "\r\n";
         
         }
         else
            stream << "Error\r\n";

         stream.flush();

         return true;
      }

      return false;
   };

}
