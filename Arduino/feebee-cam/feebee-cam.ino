#include <feebee-cam.h>
#include <esp_system.h>


void setup() {
/*
    {
        FeebeeCam::initializeSerial();
        FeebeeCam::initializeBattery();
        FeebeeCam::initializeLight();
        
        FeebeeCam::light->turnOn();

        FeebeeCam::initializeSetup();
        FeebeeCam::initializeFileSystem();
        FeebeeCam::initializeWiFi();
        FeebeeCam::initializeSettings();

        bool success = true;

        // Upload weather report with frame buffer
        FeebeeCam::uploadWeatherReport();

        if (!FeebeeCam::settings["wakeup"]) 
        {
            std::cerr << "Setting interrupt for 10 seconds" << std::endl;
            
            FeebeeCam::initializeRTC();
            FeebeeCam::RTC.setAlarmIRQ(10);

            std::cerr << "Powering down" << std::endl;

            FeebeeCam::light->turnOff();

            bat_disable_output();

            esp_deep_sleep(1000L * 1000L * 5);

        }

        return;

    }
*/
    FeebeeCam::initializeBattery();
    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    
    FeebeeCam::initializeLight();
    
    FeebeeCam::light->turnOn();
    

    FeebeeCam::initializeSetup();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeWiFi();
    FeebeeCam::initializeSettings();
    FeebeeCam::initializeRTC();

    FeebeeCam::initializeCommands();

    FeebeeCam::setLastTimeCameraUsed();
       
    if (!FeebeeCam::settings["wakeup"]) {
        // Upload weather report with frame buffer
        FeebeeCam::uploadWeatherReport();
        
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
    
    if (esp_reset_reason() == ESP_RST_TASK_WDT) {
        std::cerr << "Camera watch dog triggered" << std::endl;
        FeebeeCam::putToSleep();
    }

    delay(10);
    
}

