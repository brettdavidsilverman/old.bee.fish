#include <feebee-cam.h>
#include <esp_system.h>

void setup() {

    FeebeeCam::initializeMemory();
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeSetup();
    FeebeeCam::initializeLight();
    FeebeeCam::initializeBattery();
    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeCommands();
    FeebeeCam::initializeWiFi();
    FeebeeCam::initializeWebServer();

    if (esp_reset_reason() == ESP_RST_TASK_WDT) {
        std::cerr << "Camera watch dog triggered" << std::endl;
        FeebeeCam::putToSleep();
    }

       
}


void loop() {

    FeebeeCam::handleCommandLine();
    FeebeeCam::handleCommands();
    
    delay(10);
    
}

namespace FeebeeCam {

    void onConnectedToInternet() {
        
        std::clog << "Enter command:" << std::endl;

        delay(1000);

        if (Serial.available()) {
            std::clog << "Entered command line mode. Type help. Type restart" << std::endl;
            while (1) {
                FeebeeCam::handleCommandLine();
                delay(10);
            }
        }

        FeebeeCam::initializeRTC();

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

        FeebeeCam::initializeSettings();

        if (!settings["wakeup"]) {
            // Upload weather report with frame buffer
            success &= FeebeeCam::uploadWeatherReport();

            // if successfull, put back to sleep
            success &= FeebeeCam::putToSleep();
        }

   }

}