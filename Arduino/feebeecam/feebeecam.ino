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
   
   Light light;
   
   light.rainbow();

   psramInit();

   printMemory();

   bat_init();

   initializeFileSystem();
   initializeCamera(2);
   initializeWiFi();
   initializeWebServer();
   printMemory();

   light.turnOff();
}

double getFramerate() {
   
   double framerate = FeebeeCam::framesPerSecond;
   
   FeebeeCam::lastTimeFramesCounted = esp_timer_get_time();
   FeebeeCam::frameCount = 0;

   return framerate;
}

void loop() {

   if (FeebeeCam::downloadWhenReady) {
      FeebeeCam::downloadWhenReady = false;
      downloadRequiredFiles();
   }

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
   Serial.println("Initializing WiFi");
   
   WiFi.mode(WIFI_AP_STA);
   WiFi.onEvent(clientConnected, ARDUINO_EVENT_WIFI_AP_STACONNECTED);
   WiFi.onEvent(gotIPAddress, ARDUINO_EVENT_WIFI_STA_GOT_IP);
   WiFi.softAPConfig(IPAddress(10, 10, 1, 1), IPAddress(10, 10, 1, 1), IPAddress(255, 255, 255, 0));
   WiFi.softAP(ACCESS_POINT_SSID, PASSWORD);
   
   // attempt to connect to Wifi network:
   Setup setup;

   if (setup._ssid.length()) {

      std::cout << "Connecting to ssid " 
           << setup._ssid.c_str() 
           << "(" << setup._ssid.length() << ")" 
           << ", "
           << setup._password.c_str() 
           << "(" << setup._password.length() << ")" 
           << std::endl;

      if (setup._ssid == INTERNET_SSID)
         Serial.println("SSID Ok");

      if (setup._password == PASSWORD)
         Serial.println("Password Ok");
      
      if (setup._password.length() == 0)
         WiFi.begin(setup._ssid.c_str());
      else
         WiFi.begin(setup._ssid.c_str(), setup._password.c_str());

   }
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

   webServer1->requests()["/capture"]  = onCaptureGet;
   webServer1->requests()["/command"]  = onCommandPost;
   webServer1->requests()["/settings"] = onSettings;
   webServer1->requests()["/setup"]    = onSetup;
   webServer1->requests()["/light"]    = onLight;

   webServer1->requests()["/weather"]  =
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

   webServer1->requests()["/restart"] = 
      [](BeeFishWeb::WebRequest& request, WiFiClient& client) {
         ESP.restart();
         return true;
      };


   webServer1->setDefaultRequest(onFileServer);

}

