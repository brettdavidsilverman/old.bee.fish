#include <feebee-cam.h>

void setup() {

    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeLight();

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
/*
    We are not putting us to sleep automatically because
    it could "brick" the device.

    unsigned long millisSinceCameraLastUsed = millis() - FeebeeCam::lastTimeCameraUsed;
    
    if (millisSinceCameraLastUsed > (5L * 60L * 1000L)) {
        cerr << "millisSinceCameraLastUsed: " << millisSinceCameraLastUsed << endl;
        FeebeeCam::putToSleep();
    }
*/
    delay(10);
    
}