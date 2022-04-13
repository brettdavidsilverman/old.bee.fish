#include <Arduino.h>
#include <feebee-cam.h>

using namespace FeebeeCam;

void initializeSerial();

void setup() {

   
   initializeSerial();

   Light light;
   
   light.rainbow();

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

         BeeFishWebRequest::logon(setup._secretHash);

         Serial.println("Uploading beehive IP Address");

         BeeFishJSONOutput::Object object;

         object["ssid"] = setup._ssid;
         object["label"] = setup._label;
         object["url"] = "http://" + BString(WiFi.localIP().toString().c_str()) + "/";

         if (BeeFishStorage::setItem("/beehive/", "beehive", object))
            Serial.println("Storage at " HOST "/beehive/?beehive set");
         else
            Serial.println("Error setting " HOST "/beehive/?beehive");
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
      else if (line.startsWith("file")) {
         BString file = line.substr(line.find(' ') + 1);
         downloadFile(file, "/tmp", true);
      }
      else if (line == "weather") {
         Weather weather;
         const BeeFishJSONOutput::Object& object = weather.getWeather();
         cout << object << endl;
      }
   }
}


void initializeSerial() {

   Serial.begin(115200);

   while (!Serial) {
      delay(10);
   }

   Serial.println("Starting...");

}