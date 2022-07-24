#include <feebee-cam.h>

void setup() {

    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeLight();

    FeebeeCam::initializeBattery();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCamera();
    FeebeeCam::initializeCommands();
    FeebeeCam::initializeWiFi();

    //FeebeeCam::initializeWebServer();
    FeebeeCam::lastTimeCameraUsed = millis();

}


void loop() {

    FeebeeCam::handleCommandLine();
    FeebeeCam::handleCommands();

    if ((millis() - FeebeeCam::lastTimeCameraUsed) > (2L * 60L * 1000L)) {
        FeebeeCam::putToSleep();
    }

    delay(10);
    
}