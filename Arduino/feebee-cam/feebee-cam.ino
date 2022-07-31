#include <feebee-cam.h>

void setup() {

    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeLight();
    
    WiFi.disconnect(true, true);

    FeebeeCam::initializeBattery();
    FeebeeCam::initializeFileSystem();
    //FeebeeCam::initializeCamera(); // Initialized on wake up
    FeebeeCam::initializeCommands();
    FeebeeCam::initializeWiFi();


    //FeebeeCam::initializeWebServer();
    FeebeeCam::lastTimeCameraUsed = millis();

}


void loop() {

    FeebeeCam::handleCommandLine();
    FeebeeCam::handleCommands();
    

    unsigned long millisSinceCameraLastUsed = millis() - FeebeeCam::lastTimeCameraUsed;
    
    if ( FeebeeCam::setup._secretHash.length() &&
         millisSinceCameraLastUsed > (5L * 60L * 1000L) )
    {
        cerr << "millisSinceCameraLastUsed: " << millisSinceCameraLastUsed << endl;
        FeebeeCam::putToSleep();
    }

    delay(10);
    
}