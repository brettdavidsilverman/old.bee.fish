#include "battery.h"
#include "esp_camera.h"
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include <feebee-cam.h>

#include "camera_pins.h"

void startCameraServer();
bool initializeWebServers();

void setup() {
//    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // disable   detector
    
    
    FeebeeCam::initializeSerial();
    FeebeeCam::initializeBattery();

    FeebeeCam::initializeMemory();

    FeebeeCam::initializeFileSystem();
    FeebeeCam::initializeSetup();

    FeebeeCam::initializeLight();
    FeebeeCam::initializeCamera();
    FeebeeCam::initializeWeather();

    FeebeeCam::initializeCommands();
    FeebeeCam::initializeWiFi();

    /* Initialize commands slowws frame rate */
    if (FeebeeCam::isConnectedToInternet) {
        if (FeebeeCam::SSLConnection::test("bee.fish", 443, 10))
            std::cerr << "Success" << std::endl;
        else
            std::cerr << "Error" << std::endl;
    }

    FeebeeCam::resetCameraWatchDogTimer();


}

bool initializeWebServers() {

    std::cerr << "::initializeWebServers " << std::endl;

    startCameraServer();

    return true;
}

namespace FeebeeCam {


    bool onConnectedToInternet() {
        std::cerr << "Connected to internet" << std::endl;
        //FeebeeCam::initializeStatus();
        //FeebeeCam::status.save();
        return true;
    }

}

void loop() {
    
    if ( FeebeeCam::_setup->_isSetup && 
         millis() >= FeebeeCam::cameraWatchDogTimer )
    {
        std::cerr << "Camera watch dog triggered" << std::endl;
        FeebeeCam::resetCameraWatchDogTimer();
        
        RESTART_AFTER_ERROR();
    };
  
//    FeebeeCam::handleCommandLine();

}
