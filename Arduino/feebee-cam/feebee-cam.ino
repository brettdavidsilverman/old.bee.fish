#include <Arduino.h>
#include <feebee-cam.h>

using namespace FeebeeCam;

void initializeWebServer();

// Two Web Servers, one on each core
WiFiWebServer* webServer0;
WiFiWebServer* webServer1;


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

   static bool connecting = false;

   if (FeebeeCam::downloadWhenReady) {
      FeebeeCam::downloadWhenReady = false;

      Setup setup;

      if (setup._secretHash.length() > 0)
         BeeFishWebRequest::logon(setup._secretHash);

      downloadRequiredFiles();
   }

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

