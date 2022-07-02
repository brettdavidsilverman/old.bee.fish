#pragma once
#include <bee-fish.h>
#include "esp_camera.h"
#include "camera-pins.h"
#include "wifi-web-server.h"

namespace FeebeeCam {
    
    void initializeCamera();
    bool onCamera(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);
    bool onCapture(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);
    //bool onCommandPost(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);

    double getFramerate();
    void putToSleep();
            
    extern volatile bool     stop;
    extern volatile bool     isRunning;
    extern volatile bool     pause;
    extern volatile bool     isPaused;
    extern volatile float    framesPerSecond;
    extern volatile int      frameCount;
    extern volatile int64_t  lastTimeFramesCounted;
    extern bool cameraInitialized;
    extern volatile bool     _putToSleep;
}