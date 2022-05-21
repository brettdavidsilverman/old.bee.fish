#include <Arduino.h>
#include <feebee-cam.h>

using namespace FeebeeCam;

void initializeSerial();
bool handleCommandLine();

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

   light->turnOff();

}

void loop() {

   static bool connecting = false;

   if (FeebeeCam::downloadWhenReady) {

      FeebeeCam::downloadWhenReady = false;

      downloadRequiredFiles();
      initializeSettings();

   }

   static unsigned long weatherReportTime = 0;

   if (  millis() > weatherReportTime  &&
         _setup._secretHash.length() &&
         FeebeeCam::connectedToInternet ) {
            
      uploadWeatherReport();

      weatherReportTime = millis() + 20000;

   }

   handleCommandLine();

   if (_putToSleep)
      putToSleep();

   delay(10);

}


void initializeSerial() {

   Serial.begin(1500000);

   while (!Serial) {
      delay(10);
   }

 
}

bool handleCommandLine() {

   if (!Serial.available())
      return false;

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
      if (downloadFile(file, "/tmp.txt", true, true))
         cout << "File downloaded as /tmp.txt" << endl;
   }
   else if (line == "weather") {
      const BeeFishBScript::Object& object = weather.getWeather();
      cout << object << endl;
   }
   else if (line == "logon") {
      if (BeeFishWebRequest::logon(_setup._secretHash))
         Serial.println("Logged on");
      else
         Serial.println("Error loggong on");
   }

   return true;

}

