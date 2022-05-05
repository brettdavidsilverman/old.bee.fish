#include <Arduino.h>
#include <feebee-cam.h>

using namespace FeebeeCam;

void initializeSerial();

void setup() {

   
   initializeSerial();
   Serial.println("Starting...");

   //initializeMultiplexer();

   light.initialize();
   light.turnOn();
   
   weather.initialize();

   psramInit();

   initializeBattery();
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

      Setup setup;

      if (setup._secretHash.length() > 0) {

         Serial.println("Logging on to " HOST);

         if (BeeFishWebRequest::logon(setup._secretHash)) {

            Serial.println("Uploading beehive IP Address");

            BeeFishBScript::Object object;

            object["ssid"] = setup._ssid;
            object["label"] = setup._label;
            object["url"] = "http://" + BString(WiFi.localIP().toString().c_str()) + "/";

            if (BeeFishStorage::setItem("/beehive/", "beehive", object))
               Serial.println("Storage at " HOST "/beehive/?beehive set");
            else
               Serial.println("Error setting " HOST "/beehive/?beehive");

         }

         settings.initialize();
         settings["quality"] = 11;

         Serial.print("Settings.quality");
         Serial.println((BeeFishBScript::Number)(settings["quality"]));
      }

      downloadRequiredFiles();

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
         settings.initialize();
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