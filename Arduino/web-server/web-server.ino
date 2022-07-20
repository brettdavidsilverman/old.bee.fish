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
//    FeebeeCam::initializeWebServer();

}


void loop() {

    static unsigned long uploadWeatherReportTime = 0;
    const unsigned long weatherReportInterval = 10000;//300000;// (5 minutes)

    if (FeebeeCam::connectedToInternet) {

        if (    !FeebeeCam::isCameraRunning && 
                (millis() > uploadWeatherReportTime) )
        {

            //FeebeeCam::uploadWeatherReport();

            FeebeeCam::commands.push(FeebeeCam::UPLOAD_WEATHER);
            uploadWeatherReportTime = millis() + weatherReportInterval;
        }
    }

    FeebeeCam::handleCommandLine();

    //delay(10);
    
}


