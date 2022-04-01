#include <Arduino.h>
#include <feebee-cam.h>

using namespace FeebeeCam;


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

void loop() {

   static bool connecting = false;

   if (FeebeeCam::downloadWhenReady) {
      FeebeeCam::downloadWhenReady = false;

      Setup setup;

      if (setup._secretHash.length() > 0) {
         BeeFishWebRequest::logon(setup._secretHash);

         BeeFishJSONOutput::Object object;

         object["label"] = setup._label;
         object["url"] = "http://" + BString(WiFi.localIP().toString().c_str()) + "/";

         BeeFishStorage::setItem("/client/storage/", "beehive", object);
      }
/*
//      downloadRequiredFiles();
      Serial.print("Downloading error.js ");
      if (downloadFile("/beehive/error.js", "/error.js")) {
         Serial.println("OK");
      }
      else {
         Serial.println("Faise");
      }
*/
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

