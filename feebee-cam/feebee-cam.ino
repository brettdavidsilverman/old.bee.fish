#include <feebee-cam.h>
#include <esp_system.h>
#include <unistd.h>


void setup() {
    
    FeebeeCam::initializeSerial();
    
    FeebeeCam::initializeMemory();
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

        //FeebeeCam::downloadFiles(false, true);
        
        // Reinitialize the multiplexer after accessing rtc wire
        //FeebeeCam::initializeMultiplexer();
        //FeebeeCam::initializeLight();

        if (FeebeeCam::_setup->_isSetup) {

            if (!FeebeeCam::BeeFishWebRequest::logon(FeebeeCam::_setup->_secretHash))
                FeebeeCam::restartAfterError();

            FeebeeCam::initializeSettings();

            if (  FeebeeCam::settings.contains("wakeup") &&
                 !FeebeeCam::settings["wakeup"] )
            {
                unsigned long takePictureEvery;

                if (!settings.contains("takePictureEvery"))
                    settings["takePictureEvery"] = TAKE_PICTURE_EVERY;

                takePictureEvery = 
                    (double)settings["takePictureEvery"] ;

                std::chrono::system_clock::time_point timeNow 
                    = std::chrono::system_clock::now();

                unsigned long epoch =
                    std::chrono::duration_cast<std::chrono::seconds>(
                    timeNow.time_since_epoch()).count();

                if (lastTimePictureTaken + takePictureEvery < epoch) {
                    // Upload weather report with frame buffer
                    FeebeeCam::uploadImage();

                    lastTimePictureTaken = epoch;
                }

                // Upload weather report
                FeebeeCam::uploadWeatherReport();

                // if successfull, put back to sleep
                // putToSleep saves settings before sleeping
                if (FeebeeCam::_setup->_isSetup)
                    FeebeeCam::putToSleep();
            }

            FeebeeCam::BeeFishStorage storage("/beehive/");

            FeebeeCam::settings["sleeping"] = false;
            
            FeebeeCam::settings.save();

            FeebeeCam::light->turnOff();
        }

        cerr << "Awake and awaiting you at " << FeebeeCam::getURL() << endl;

        return true;
    }

}