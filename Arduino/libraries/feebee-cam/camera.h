#pragma once
#include <bee-fish.h>
#include "esp_camera.h"
#include "camera-pins.h"
#include "wifi-web-server.h"

namespace FeebeeCam {
    
    void initializeCamera(size_t frameBufferCount = 2);

    bool onCameraGet(BeeFishWeb::WebRequest& request, WiFiClient& client);
    bool onCaptureGet(BeeFishWeb::WebRequest& request, WiFiClient& client);
    bool onCommandPost(BeeFishWeb::WebRequest& request, WiFiClient& client);
    bool onSettings(BeeFishWeb::WebRequest& request, WiFiClient& client);
    bool saveSettings();
    bool retrieveSettings();
    double getFramerate();
            
    extern volatile bool     stop;
    extern volatile bool     isRunning;
    extern volatile bool     pause;
    extern volatile bool     isPaused;
    extern volatile float    framesPerSecond;
    extern volatile int      frameCount;
    extern volatile int64_t  lastTimeFramesCounted;
    extern bool cameraInitialized;
}