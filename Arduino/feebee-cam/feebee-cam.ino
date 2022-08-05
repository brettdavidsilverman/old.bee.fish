#include <feebee-cam.h>

void setup() {

    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();

    std::clog << "Press any key to enter command line mode" << std::endl;

    FeebeeCam::initializeLight();
    FeebeeCam::initializeBattery();
    FeebeeCam::initializeFileSystem();
    //FeebeeCam::initializeCamera(); // Initialized on wake up
    FeebeeCam::initializeCommands();
    FeebeeCam::initializeWiFi();


    //FeebeeCam::initializeWebServer();
    FeebeeCam::lastTimeCameraUsed = millis();


    if (Serial.available()) {
        std::clog << "Entered command line mode. Type help. Type restart" << std::endl;
        while (1) {
            FeebeeCam::handleCommandLine();
            delay(10);
        }
    }

       
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