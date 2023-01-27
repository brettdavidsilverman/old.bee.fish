#include "light.h"
#include "blue-light.h"
#include "feebee-cam.h"

namespace FeebeeCam {   

   Light light;
   BlueLight blueLight;

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
         <<   "<head>"
         <<      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=1\"/>"
         <<   "</head>"
         <<   "<body>"
         <<      "<button onclick='toggleLight(false)'>Toggle Light</button>"
         <<      "<button onclick='toggleLight(true)'>Toggle Flash</button>"
         <<        "<br />"
         <<        "<div id=\"response\"></div>"
         <<        "<script>"
         << "function toggleLight(flash) {"
         << "  var body;"
         << "  if (flash)"
         << "    body = {\"flash\": \"toggle\"};"
         << "  else"
         << "    body = {\"light\": \"toggle\"};"
         << "  var params = {"
         << "    method: \"POST\","
         << "    body: JSON.stringify(body),"
         << "    headers: "
         << "      new Headers(["
         << "         [\"Content-Type\", \"application/json; charset=utf-8\"]"
         << "      ])"
         << "  };"
         << "  fetch('/light', params).then("
         << "    function(response) {"
         << "      return response.text();"
         << "    }"
         << "  ).then("
         << "    function(text) {"
         << "      document.getElementById('response').innerHTML = text;"
         << "    }"
         << "  ).catch("
         << "    function(error) {"
         << "      document.getElementById('response').innerHTML = error;"
         << "      toggleLight();"
         << "    }"
         << "  );"
         << "}"
         <<     "</script>"
         <<   "</body>"
         << "</html>\r\n";

         stream.flush();

         light.turnOn();

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
            light.toggleFlash();
         else
            light.toggle();

         BeeFishBScript::Object object {
            {"light", light.status()},
            {"flash", light.flashStatus()}
         };

         stream << object;
       
         stream.flush();

         return true;
      }

      return false;
   };

   bool initializeLight() {
      
      light.initialize();

      light.flash(200, 1);
      
      light.turnOff();

      blueLight.initialize();

      blueLight.flash(200, 1);

      return true;
   }



}
