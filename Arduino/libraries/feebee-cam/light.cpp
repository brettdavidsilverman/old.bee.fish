#include <pimoroni-light.h>

namespace FeebeeCam {   

   PimoroniLight customLight;
   Light& light = customLight;
    
    bool onLight(BeeFishWeb::WebRequest& request, WiFiClient& client) {
      if (request.method() == "GET") {
         client.println("HTTP/1.1 200 OK");

         client.println("Content-Type: text/html;charset=utf-8");
         client.println("Access-Control-Allow-Origin: null");
         client.println("Connection: keep-alive");

         client.println();

         client.println(
            "<html>"
               "<head>"
                  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=1\"/>"
               "</head>"
               "<body>"
                  "<button onclick='toggleLight()'>Toggle Light</button>"
                  "<br />"
                  "<div id=\"response\"></div>"
                  "<script>"
            "function toggleLight() {"
            "  var params = {"
            "     method: \"POST\","
            "     body: \"\\\"toggle\\\"\""
            "  };"
            "  fetch('/light', params).then("
            "     function(response) {"
            "        return response.text();"
            "     }"
            "  ).then("
            "     function(text) {"
            "        document.getElementById('response').innerHTML = text;"
            "     }"
            "  ).catch("
            "     function(error) {"
            "        document.getElementById('response').innerHTML = error;"
            "        toggleLight();"
            "     }"
            "  );"
            "}"
                  "</script>"
               "</body>"
            "</html>"
         );

         light.turnOn();

         return true;
      }
      else if (request.method() == "POST") {

         BeeFishJSON::JSON json;
         BeeFishJSON::JSONParser parser(json);

         while (client.available() && json.result() == BeeFishMisc::nullopt) {
            char c = client.read();
            Serial.print(c);
            parser.match(c);
         }

         client.println("HTTP/1.1 200 OK");

         client.println("Content-Type: text/plain;charset=utf-8");
         client.println("Access-Control-Allow-Origin: null");
         client.println("Connection: keep-alive");

         client.println();

         if (json.result() == true) {

            light.toggle();

            BeeFishBScript::Object object {
               {"status", light.status()}
            };

            client.println(object.str().c_str());
         
         }
         else
            client.println("Error");

         return true;
      }

      return false;
   };

}
