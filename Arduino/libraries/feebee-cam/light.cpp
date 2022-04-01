#include "light.h"

namespace FeebeeCam {
    
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
                  "Red<br/>"
                  "<input id=\"red\" type=\"range\" min=\"0\" max=\"255\" onchange=\"postRGB()\"></input><br/>"
                  "Green<br/>"
                  "<input id=\"green\" type=\"range\" min=\"0\" max=\"255\" onchange=\"postRGB()\"></input><br/>"
                  "Blue<br/>"
                  "<input id=\"blue\" type=\"range\" min=\"0\" max=\"255\" onchange=\"postRGB()\"></input><br/>"
                  "Choose<br/>"
                  "<input type=\"color\" id=\"color\" onchange=\"postColor(this)\"></input><br/>"
                  "Brightness<br/>"
                  "<input id=\"brightness\" type=\"range\" min=\"0\" max=\"255\" onchange=\"postRGB()\"></input><br/>"
                  "<pre id=\"response\"></pre>"
                  "<script>"
            "function postRGB() {"
            "  post("
            "     document.getElementById('red').value,"
            "     document.getElementById('green').value,"
            "     document.getElementById('blue').value,"
            "     document.getElementById('brightness').value"
            "  );"
            "}"
            "function postColor(input) {"
            "  var hex = input.value; /*#rrggbb*/"
            "  var red = parseInt(hex.substr(1, 2), 16);"
            "  var green = parseInt(hex.substr(3, 2), 16);"
            "  var blue = parseInt(hex.substr(5, 2), 16);"
            "  var brightness = Number(document.getElementById('brightness').value);"
            "  post(red, green, blue, brightness);"
            "}"
            "function post(red, green, blue, brightness) {"
            "  document.getElementById('red').value = red;"
            "  document.getElementById('green').value = green;"
            "  document.getElementById('blue').value = blue;"
            "  document.getElementById('brightness').value = brightness;"
            "  var hex = "
            "           Number(red).toString(16).padStart(2, '0') + "
            "           Number(green).toString(16).padStart(2, '0') + "
            "           Number(blue).toString(16).padStart(2, '0');"
            "  document.getElementById('color').value = '#' + hex;"
            "  params = {"
            "     method: \"POST\","
            "     body: JSON.stringify({"
            "        red: Number(red),"
            "        green: Number(green),"
            "        blue: Number(blue),"
            "        brightness: Number(brightness)"
            "     })"
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
            "        post(red, green, blue, brightness);"
            "     }"
            "  );"
            "}"
                  "</script>"
               "</body>"
            "</html>"
         );

         Light light;
         light.rainbow();

         return true;
      }
      else if (request.method() == "POST") {

         BeeFishJSON::JSON json;
         BeeFishJSON::JSONParser parser(json);

         BeeFishMisc::optional<BeeFishBString::BString> red;
         BeeFishMisc::optional<BeeFishBString::BString> green;
         BeeFishMisc::optional<BeeFishBString::BString> blue;
         BeeFishMisc::optional<BeeFishBString::BString> brightness;

         parser.captureValue("red", red);
         parser.captureValue("green", green);
         parser.captureValue("blue", blue);
         parser.captureValue("brightness", brightness);

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
            uint8_t _red = 0;
            uint8_t _green = 0;
            uint8_t _blue = 0;
            uint8_t _brightness = 0;

            if (red.hasValue())
               _red = atoi(red.value().c_str());

            if (green.hasValue())
               _green = atoi(green.value().c_str());

            if (blue.hasValue())
               _blue = atoi(blue.value().c_str());

            if (brightness.hasValue()) {
               _brightness = atoi(brightness.value().c_str());
            }

            BeeFishJSONOutput::Object object {
               {"red", _red},
               {"green", _green},
               {"blue", _blue},
               {"brightness", _brightness}
            };

            client.println(object.str().c_str());

            Light light;
            if (_red == 0 && _green == 0 && _blue == 0)
               light.turnOff();
            else {
               light.turnOn(_red, _green, _blue, _brightness);
            }

         }
         else
            client.println("Error");

         return true;
      }

      return false;
   };

}
