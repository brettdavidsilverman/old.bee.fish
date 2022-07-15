#include <feebee-cam.h>

void setup() {

    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeLight();
    FeebeeCam::initializeBattery();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCamera();
    FeebeeCam::initializeWiFi();
    FeebeeCam::initializeCommands();
//    FeebeeCam::initializeWebServer();

}


void loop() {

    if (FeebeeCam::downloadWhenReady) {
        
        FeebeeCam::downloadWhenReady = false;

        Serial.println("Connected to WiFi");

        FeebeeCam::commands.push(FeebeeCam::INITIALIZE);

    }

    static unsigned long uploadWeatherReportTime = 0;
    const unsigned long weatherReportInterval = 10000; //300000;// (5 minutes)

    if (FeebeeCam::connectedToInternet) {
        if (uploadWeatherReportTime < millis()) {
            FeebeeCam::commands.push(FeebeeCam::UPLOAD_WEATHER);
            uploadWeatherReportTime = millis() + weatherReportInterval;
        }
    }

    FeebeeCam::handleCommandLine();

    FeebeeCam::commandLoop(nullptr);

    //delay(10);
    
}


