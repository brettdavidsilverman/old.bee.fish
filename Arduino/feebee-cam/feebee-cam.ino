#include <feebee-cam.h>
#include <esp_system.h>


void setup() {
    bool success = true;

    success &= FeebeeCam::initializeSerial();
    success &= FeebeeCam::initializeMemory();
    success &= FeebeeCam::initializeBattery();
    success &= FeebeeCam::initializeRTC(false);
    
    std::cout << FeebeeCam::getDateTime() << std::endl;


    success &= FeebeeCam::initializeSetup();
    success &= FeebeeCam::initializeLight();
    
    success &= FeebeeCam::initializeFileSystem();
    success &= FeebeeCam::initializeCommands();
    success &= FeebeeCam::initializeWiFi();
    success &= FeebeeCam::initializeSettings();
    success &= FeebeeCam::setLastTimeCameraUsed();
       
    FeebeeCam::light->turnOff();

    if (success) {
        FeebeeCam::light->turnOn();
    }
    else {
        FeebeeCam::restartAfterError();
    }

    if (!FeebeeCam::settings["wakeup"]) {
        // Upload weather report with frame buffer
        FeebeeCam::uploadImage();
        
        FeebeeCam::light->turnOff();

        // if successfull, put back to sleep
        FeebeeCam::putToSleep();
    }

    FeebeeCam::BeeFishStorage storage("/beehive/");
    FeebeeCam::settings["sleeping"] = false;
    storage.setItem("settings", FeebeeCam::settings);

    std::clog << "Enter command:" << std::endl;

    FeebeeCam::light->turnOff();

}


void loop() {

    FeebeeCam::handleCommandLine();
    FeebeeCam::handleCommands();

    if (FeebeeCam::dnsServer)
        FeebeeCam::dnsServer->processNextRequest();

    if (esp_reset_reason() == ESP_RST_TASK_WDT) {
        std::cerr << "Camera watch dog triggered" << std::endl;
        FeebeeCam::putToSleep();
    }

    delay(10);
    
}

namespace FeebeeCam {

    void onConnectedToInternet() {
        FeebeeCam::initializeRTC(true);
    }

}