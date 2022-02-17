#include <SPI.h>
#include <WiFi.h>
#include <web-server.h>
#include <light.h>

#define INTERNET_SSID "Android"         // your network SSID (name)
#define PASSWORD "feebeegeeb3"    // your network password

int keyIndex = 0;                         // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;


WebServer server(80);
Light* light;

void webServerTask( void * pvParameters ) {
   server.begin();
   for (;;) {
      // listen for incoming clients
      server.loop();
      delay(10);
   }
}


void setup() {

   Serial.begin(1500000);

   while (!Serial) {
      delay(10);
   }

   Serial.println("Starting...");
     
   server.requests()["/"] = [light](BeeFishWeb::WebRequest& request, WiFiClient& client) {
      if (request.method() == "GET") {
         client.println("HTTP/1.1 200 OK");

         client.println("Content-Type: text/html;charset=utf-8");
         client.println("Access-Control-Allow-Origin: null");

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
                  "<pre id=\"response\"></pre>"
                  "<script>"
            "function postRGB() {"
            "  post("
            "     document.getElementById('red').value,"
            "     document.getElementById('green').value,"
            "     document.getElementById('blue').value"
            "  );"
            "}"
            "function postColor(input) {"
            "  var hex = input.value; /*#rrggbb*/"
            "  var red = parseInt(hex.substr(1, 2), 16);"
            "  var green = parseInt(hex.substr(3, 2), 16);"
            "  var blue = parseInt(hex.substr(5, 2), 16);"
            "  post(red, green, blue);"
            "}"
            "function post(red, green, blue) {"
            "  document.getElementById('red').value = red;"
            "  document.getElementById('green').value = green;"
            "  document.getElementById('blue').value = blue;"
            "  var hex = "
            "           Number(red).toString(16).padStart(2, '0') + "
            "           Number(green).toString(16).padStart(2, '0') + "
            "           Number(blue).toString(16).padStart(2, '0');"
            "  document.getElementById('color').value = '#' + hex;"
            "  params = {"
            "     method: \"POST\","
            "     body: JSON.stringify({"
            "        red: red,"
            "        green: green,"
            "        blue: blue"
            "     })"
            "  };"
            "  fetch('/', params).then("
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
            "        post(red, green, blue);"
            "     }"
            "  );"
            "}"
                  "</script>"
               "</body>"
            "</html>"
         );

         light->rainbow();

         return true;
      }
      else if (request.method() == "POST") {

         BeeFishJSON::JSON json;
         BeeFishJSON::JSONParser parser(json);

         BeeFishMisc::optional<BeeFishBString::BString> red;
         BeeFishMisc::optional<BeeFishBString::BString> green;
         BeeFishMisc::optional<BeeFishBString::BString> blue;

         parser.captureValue("red", red);
         parser.captureValue("green", green);
         parser.captureValue("blue", blue);

         while (client.available() && json.result() == BeeFishMisc::nullopt) {
            char c = client.read();
            Serial.print(c);
            parser.match(c);
         }

         client.println("HTTP/1.1 200 OK");

         client.println("Content-Type: text/plain;charset=utf-8");
         client.println("Access-Control-Allow-Origin: null");

         client.println();

         if (json.result() == true) {
            uint8_t _red = 0;
            uint8_t _green = 0;
            uint8_t _blue = 0;

            if (red.hasValue())
               _red = atoi(red.value().c_str());

            if (green.hasValue())
               _green = atoi(green.value().c_str());

            if (blue.hasValue())
               _blue = atoi(blue.value().c_str());

            client.println("Ok");
            client.print("Red:   ");
            client.println(_red);
            client.print("Green: ");
            client.println(_green);
            client.print("Blue:  ");
            client.println(_blue);

            if (_red == 0 && _green == 0 && _blue == 0)
               light->turnOff();
            else
               light->turnOn(_red, _green, _blue);

         }
         else
            client.println("Error");

         return true;
      }

      return false;
   };

   Serial.println("Connecting to " INTERNET_SSID);
   
   // attempt to connect to Wifi network:
   WiFi.begin(INTERNET_SSID, PASSWORD);

   while (!WiFi.isConnected()) {
      Serial.print("."); 
      delay(500);
   }

   BaseType_t xReturned;
   TaskHandle_t xHandle = NULL;

   xReturned = xTaskCreatePinnedToCore(
         webServerTask,         /* Task function. */
         "WebServerTask",      /* String with name of task. */
         10000,               /* Stack size in bytes. */
         NULL,                /* Parameter passed as input of the task */
         1,                     /* Priority of the task. */
         &xHandle            /* Task handle. */,
         1                      /* Pinned to core */
   );         

   light = new Light();

   light->rainbow();

   // you're connected now, so print out the status:

   printWifiStatus();
}

void loop() {
   if (!WiFi.isConnected()) {
      WiFi.begin();
      delay(3000);
   }
   delay(500);
}

void printWifiStatus() {

   // print the SSID of the network you're attached to:

   Serial.print("SSID: ");

   Serial.println(WiFi.SSID());

   // print your WiFi shield's IP address:

   IPAddress ip = WiFi.localIP();

   Serial.print("IP Address: ");

   Serial.println(ip);

   // print the received signal strength:

   long rssi = WiFi.RSSI();

   Serial.print("signal strength (RSSI):");

   Serial.print(rssi);

   Serial.println(" dBm");
}