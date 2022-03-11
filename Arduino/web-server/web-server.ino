#include <Arduino.h>
#include <WiFi.h>
#include <feebeecam.h>

#define INTERNET_SSID "Laptop"         // your network SSID (name)
#define ACCESS_POINT_SSID "FeebeeCam"
#define PASSWORD "feebeegeeb3"    // your network password

using namespace FeebeeCam;

void initializeWiFi();
void initializeWebServer();

WiFiWebServer* webServer0;
WiFiWebServer* webServer1;

void clientConnected(arduino_event_id_t event, arduino_event_info_t info) 
{

   Serial.print("Access point IP Address: ");
   IPAddress ipAddress = WiFi.softAPIP();
   Serial.println(ipAddress);

}

void gotIPAddress(arduino_event_id_t event, arduino_event_info_t info) 
{

   Serial.print("Internet IP Address: ");
   Serial.println(WiFi.localIP());
   FeebeeCam::downloadWhenReady = true;

}

void setup() {

   Serial.begin(1500000);

   while (!Serial) {
      delay(10);
   }

   Serial.println("Starting...");

   psramInit();

   printMemory();

   bat_init();

   initializeFileSystem();
   initializeCamera(2);
   initializeWiFi();
   initializeWebServer();


   Light light;
   light.rainbow();

   // you're connected now, so print out the status:

   printMemory();
//   printWifiStatus();
}

double getFramerate() {
   
   double framerate = FeebeeCam::framesPerSecond;
   
   FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
   FeebeeCam::frameCount = 0;

   return framerate;
}

void loop() {
   /*
   if (WiFi.softAPgetStationNum() == 0 && !WiFi.isConnected()) {
      //WiFi.begin();
      delay(3000);
   }
   else
   */
   if (Serial.available()) {

      String line = Serial.readString();

      while (Serial.available())
         Serial.read();

      if (line == "download")
         downloadRequiredFiles();
      else if (line == "weather") {
         Weather weather;
         const BeeFishJSONOutput::Object& object = weather.getWeather();
         cout << object << endl;
      }
   }
   if (FeebeeCam::downloadWhenReady) {
      FeebeeCam::downloadWhenReady = false;
      downloadRequiredFiles();
   }
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

void initializeWiFi() {
   Serial.println("Connecting to " INTERNET_SSID);
   
   WiFi.mode(WIFI_AP_STA);
   WiFi.onEvent(clientConnected, ARDUINO_EVENT_WIFI_AP_STACONNECTED);
   WiFi.onEvent(gotIPAddress, ARDUINO_EVENT_WIFI_STA_GOT_IP);
   WiFi.softAPConfig(IPAddress(10, 10, 1, 1), IPAddress(10, 10, 1, 1), IPAddress(255, 255, 255, 0));
   WiFi.softAP(ACCESS_POINT_SSID, PASSWORD);
   
   // attempt to connect to Wifi network:
   WiFi.begin(INTERNET_SSID, PASSWORD);

/*
   while (!WiFi.isConnected()) {
      Serial.print("."); 
      delay(500);
   }
*/
}

void initializeWebServer() {
   webServer0 = new WiFiWebServer(81, 0);
   webServer1 = new WiFiWebServer(80, 1);
   
   webServer0->requests()["/camera"] = onCameraGet;

   webServer1->requests()["/capture"] = onCaptureGet;
   webServer1->requests()["/command"] = onCommandPost;
   webServer1->requests()["/settings"] = onSettings;

   webServer1->requests()["/weather"] =
        [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
            Weather weather;
            BeeFishJSONOutput::Object& reading = weather.getWeather();

            reading["frame rate"] = BeeFishJSONOutput::Object{
               {"value", getFramerate()},
               {"unit", "fps"},
               {"precision", 2}
            };

            reading["battery"] = BeeFishJSONOutput::Object {
               {"value", bat_get_voltage()},
               {"unit", "mV"},
               {"precision", 0}
            };

            WiFiWebServer::sendResponse(client, reading);
            return true;
        };

   webServer1->requests()["/light"] = [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
      if (request.method() == "GET") {
         client.println("HTTP/1.1 200 OK");

         client.println("Content-Type: text/html;charset=utf-8");
         client.println("Access-Control-Allow-Origin: null");
         client.println("Connection: close");

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
         client.println("Connection: close");

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

   webServer1->setDefaultRequest(onFileServer);

}

