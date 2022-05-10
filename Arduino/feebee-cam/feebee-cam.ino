#include <Arduino.h>
#include <feebee-cam.h>

using namespace FeebeeCam;

void initializeSerial();

void setup() {

   initializeBattery();
   
   light.initialize();
   light.turnOn();

   initializeSerial();

   Serial.println("Starting...");

   weather.initialize();

   psramInit();

   initializeFileSystem();
   initializeCamera();
   initializeWiFi();
   initializeWebServers();

   light.turnOff();
}

void loop() {

   static bool connecting = false;

   if (FeebeeCam::downloadWhenReady) {
      FeebeeCam::downloadWhenReady = false;

      downloadRequiredFiles();

      Setup setup;

      if (setup._secretHash.length() > 0) {

         if (BeeFishWebRequest::logon(setup._secretHash)) {

            Serial.println("Synchronizing settings");

            settings.initialize();

            settings["ssid"] = setup._ssid;
            settings["label"] = setup._label;
            settings["url"] = "http://" + BString(WiFi.localIP().toString().c_str()) + "/";

            settings.save();
         }


      }

   }

   if (Serial.available()) {

      String readString = Serial.readString();
      BString line = readString.c_str();

      while (Serial.available())
         Serial.read();

      if (line == "download") {
         FeebeeCam::downloadWhenReady = true;
      }
      else if (line == "save") {
         settings.save();
      }
      
      else if (line == "settings") {
         cout << settings << endl;
      }
      else if (line.startsWith("file")) {
         BString file = line.substr(line.find(' ') + 1);
         downloadFile(file, "/tmp", true);
      }
      else if (line == "weather") {
         const BeeFishBScript::Object& object = weather.getWeather();
         cout << object << endl;
      }
   }
}


void initializeSerial() {

   Serial.begin(1500000);

   while (!Serial) {
      delay(10);
   }

 
}