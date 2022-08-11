#include <feebee-cam.h>
#include <esp_system.h>

void setup() {

    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeBattery();
    FeebeeCam::initializeSetup();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeWiFi();
    FeebeeCam::initializeSettings();
    FeebeeCam::initializeRTC();
    FeebeeCam::initializeLight();
    FeebeeCam::initializeCommands();

    FeebeeCam::setLastTimeCameraUsed();
       
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

namespace FeebeeCam {

    void onConnectedToInternet() {
        
        std::clog << "Enter command:" << std::endl;

        if (Serial.available()) {
            std::cout << "Entered command line mode. Type help. Type restart" << std::endl;
            while (1) {
                FeebeeCam::handleCommandLine();
                delay(10);
            }
        }


/*
        if (SSLConnection::test("laptop", 443))
            std::cerr << "Test OK" << std::endl;
        else {
            std::cerr << "ERROR!!!!" << std::endl;
            while (1)
                ;
        }
*/
        bool success = true;


        if (!settings["wakeup"]) {
            // Upload weather report with frame buffer
            success &= FeebeeCam::uploadWeatherReport();

            // if successfull, put back to sleep
            success &= FeebeeCam::putToSleep();
        }

   }

}