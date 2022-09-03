#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>


void setup() {
    bool success = true;
    
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeMemory();
    FeebeeCam::initializeBattery();
    //FeebeeCam::initializeRTC(false);
    FeebeeCam::initializeSetup();
    
    //std::cout << FeebeeCam::getDateTime() << std::endl;


    FeebeeCam::initializeLight();

    FeebeeCam::light->turnOn();
    
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCommands();
    FeebeeCam::initializeWiFi();
    FeebeeCam::initializeWebServer();
    //FeebeeCam::checkCommandLine();
    FeebeeCam::setLastTimeCameraUsed();
       

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

    if (esp_reset_reason() == ESP_RST_TASK_WDT) {
        std::cerr << "Camera watch dog triggered" << std::endl;
        FeebeeCam::putToSleep();
    }

    vTaskDelay(5);
    
}


namespace FeebeeCam {

    bool onConnectedToInternet() {
        
        //FeebeeCam::initializeTime();
        //FeebeeCam::initializeRTC(true);

        if (FeebeeCam::_setup->_isSetup) {

            if (!FeebeeCam::BeeFishWebRequest::logon(FeebeeCam::_setup->_secretHash))
                return false;

            FeebeeCam::initializeSettings();

            if (  FeebeeCam::settings.contains("wakeup") &&
                 !FeebeeCam::settings["wakeup"] )
            {
                // Upload weather report with frame buffer
                //FeebeeCam::uploadImage();
                
                FeebeeCam::light->turnOff();

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