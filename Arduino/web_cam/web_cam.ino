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

    FeebeeCam::_setup->_isSetup = false;

    FeebeeCam::initializeLight();
    FeebeeCam::initializeCamera();
    FeebeeCam::initializeWeather();
/* Initialize commands slowws frame rate */
    //FeebeeCam::initializeCommands();

    FeebeeCam::initializeWiFi();
  
    FeebeeCam::resetCameraWatchDogTimer();

    Serial.begin(1500000);
    while (!Serial)
        delay(10);
/*
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer   = LEDC_TIMER_0;
    config.pin_d0       = Y2_GPIO_NUM;
    config.pin_d1       = Y3_GPIO_NUM;
    config.pin_d2       = Y4_GPIO_NUM;
    config.pin_d3       = Y5_GPIO_NUM;
    config.pin_d4       = Y6_GPIO_NUM;
    config.pin_d5       = Y7_GPIO_NUM;
    config.pin_d6       = Y8_GPIO_NUM;
    config.pin_d7       = Y9_GPIO_NUM;
    config.pin_xclk     = XCLK_GPIO_NUM;
    config.pin_pclk     = PCLK_GPIO_NUM;
    config.pin_vsync    = VSYNC_GPIO_NUM;
    config.pin_href     = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn     = PWDN_GPIO_NUM;
    config.pin_reset    = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size   = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count     = 2;
    config.fb_location = CAMERA_FB_IN_PSRAM;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

*/    
/*
    sensor_t *s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    s->set_vflip(s, 1);        // flip it back
    s->set_brightness(s, 1);   // up the blightness just a bit
    s->set_saturation(s, -2);  // lower the saturation

    // drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_QVGA);


    Serial.printf("Connect to %s, %s\r\n", ssid, password);

    WiFi.begin(ssid, password);
*/
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
 

    // If you want to use AP mode, you can use the following code
    // WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.localIP();
    Serial.print("IP address: ");
    Serial.println(IP);

    startCameraServer();

}

bool initializeWebServers() {

    std::cerr << "::initializeWebServers " << std::flush;

    if (FeebeeCam::dnsServer) {
        DNSServer* _dnsServer = FeebeeCam::dnsServer;
        FeebeeCam::dnsServer = nullptr;
        _dnsServer->stop();
        delete _dnsServer;
    }

    bool success = true;

    if (FeebeeCam::isConnectedToESPAccessPoint) {
        FeebeeCam::dnsServer = new DNSServer();
        success &= FeebeeCam::dnsServer->start(53, "*", WiFi.softAPIP());
    }

    startCameraServer();

    if (success)
        std::cerr << " Ok";
    else
        std::cerr << " Fail";
    
    std::cerr << std::endl;
    
    return success;
}

namespace FeebeeCam {


    bool onConnectedToInternet() {
        std::cerr << "Connected to internet" << std::endl;
        std::cerr << FeebeeCam::getURL() << std::endl;
        return true;
    }

}

void loop() {

    if ( FeebeeCam::dnsServer ) {
         FeebeeCam::dnsServer->processNextRequest();
    }

    if ( FeebeeCam::_setup->_isSetup && 
         millis() >= FeebeeCam::cameraWatchDogTimer )
    {
        std::cerr << "Camera watch dog triggered" << std::endl;
        FeebeeCam::resetCameraWatchDogTimer();
        
        RESTART_AFTER_ERROR();
    };

    FeebeeCam::handleCommandLine();

}
