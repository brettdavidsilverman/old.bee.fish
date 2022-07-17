#include <Arduino.h>
#include <feebee-cam.h>

using namespace FeebeeCam;

bool handleCommandLine();

void setup() {

   initializeMemory();
   
   initializeSerial();

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

