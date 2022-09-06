#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>


void setup() {
    bool success = true;
    
    FeebeeCam::initializeSerial();

    FeebeeCam::initializeMemory();
    //FeebeeCam::initializeCamera(FRAME_BUFFER_COUNT);
    FeebeeCam::initializeBattery();
    FeebeeCam::initializeMultiplexer();
    FeebeeCam::initializeLight();

    FeebeeCam::light->flash(100, 1);

    FeebeeCam::initializeSetup();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCommands();
    FeebeeCam::initializeWiFi();
    FeebeeCam::initializeWebServer();
    //FeebeeCam::checkCommandLine();
    FeebeeCam::resetCameraWatchDogTimer();

    if (!success) {
        FeebeeCam::light->turnOff();
        FeebeeCam::restartAfterError();
    }

}


void loop() {
    FeebeeCam::handleCommandLine();
    FeebeeCam::handleCommands();

    if (FeebeeCam::dnsServer)
        FeebeeCam::dnsServer->processNextRequest();

    if ( millis() > FeebeeCam::cameraWatchDogTimer ) {
        std::cerr << "Camera watch dog triggered" << std::endl;
        FeebeeCam::putToSleep();
    };

    vTaskDelay(5);
    
}


namespace FeebeeCam {

    bool onConnectedToInternet() {

        cerr << "Connected to internet" << endl;

        FeebeeCam::initializeTime();
        //FeebeeCam::initializeRTC();

        if (FeebeeCam::_setup->_isSetup) {

            if (!FeebeeCam::BeeFishWebRequest::logon(FeebeeCam::_setup->_secretHash))
                FeebeeCam::restartAfterError();

            FeebeeCam::initializeSettings();

            if (  FeebeeCam::settings.contains("wakeup") &&
                 !FeebeeCam::settings["wakeup"] )
            {
                // Upload weather report with frame buffer
                FeebeeCam::uploadImage();

                // if successfull, put back to sleep
                FeebeeCam::putToSleep();
            }

            FeebeeCam::BeeFishStorage storage("/beehive/");

            FeebeeCam::settings["sleeping"] = false;
            storage.setItem("settings", FeebeeCam::settings);
            FeebeeCam::light->turnOff();
        }

        return true;
    }

}