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

}


void loop() {

    static unsigned long uploadWeatherReportTime = 0;
    const unsigned long weatherReportInterval = 10000;//300000;// (5 minutes)

    if (FeebeeCam::connectedToInternet) {


        if (    !FeebeeCam::isCameraRunning && 
                (millis() > uploadWeatherReportTime) )
        {

            FeebeeCam::commands.push(FeebeeCam::UPLOAD_WEATHER);
            uploadWeatherReportTime = millis() + weatherReportInterval;
        }
    }

    FeebeeCam::handleCommandLine();

    static long secondCount = 0;
    
    if (secondCount < millis()) {
        secondCount = millis() + 1000;
        double frameRate = FeebeeCam::getFrameRate();
        if (frameRate != 0)
            std::cerr << "Frame rate: " << frameRate << std::endl;
    }

    delay(1);
    
}


