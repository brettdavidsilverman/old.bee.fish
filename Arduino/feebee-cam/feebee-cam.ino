#include <Arduino.h>
#include <feebee-cam.h>

using namespace FeebeeCam;

void initializeSerial();

void setup() {

   psramInit();
 
   initializeSerial();
   Serial.println("Starting...");

   initializeLight();

   light->turnOn();


   initializeBattery();
   
   weather.initialize();

   initializeFileSystem();
   initializeCamera();
   initializeWiFi();
   initializeWebServers();

   Setup setup;

   if (setup._ssid.length() && setup._secretHash.length()) {

      Serial.print("Retrieving sleep settings");

      while (!WiFi.isConnected()) {
         Serial.print(".");
         delay(500);
      }


      if (BeeFishWebRequest::logon(setup._secretHash)) {
         
         BeeFishStorage storage;

         settings.initialize();

         if (settings["wakeup"]) {
            Serial.println("Waking up....");
            light->turnOff();
            return;
         }
         
         BeeFishId::Id id;

         cout << "Weather Id: " << id << endl;

         storage.setItem("/beehive/weather/", id, weather.getWeather());

         light->turnOff();

         const long sleepTime = 10L * 1000L * 1000L;

         Serial.print("Timer sleep ");
         Serial.println(sleepTime);

         esp_sleep_enable_timer_wakeup(sleepTime);
         
         esp_deep_sleep_start();
      }

   }

   light->turnOff();

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

            settings["awake"] = true;
            settings["ssid"] = setup._ssid;
            settings["label"] = setup._label;
            settings["url"] = "http://" + BString(WiFi.localIP().toString().c_str()) + "/";

            settings.save();
         }
      }

   }

   if (putToSleep) {
      Serial.println("Putting to sleep");
      settings.initialize();
      settings["wakeup"] = false;
      settings["awake"] = false;
      settings.save();
      ESP.restart();
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