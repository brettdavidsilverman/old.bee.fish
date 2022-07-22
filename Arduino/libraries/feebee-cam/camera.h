#pragma once
#include <bee-fish.h>
#include "esp_camera.h"
#include "camera-pins.h"
#include "web-server2.h"

namespace FeebeeCam {
    
    bool initializeCamera();
    bool onCamera(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onCapture(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onCommand(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

    double getFrameRate();
            
    extern volatile bool            stop;
    extern volatile bool            isCameraRunning;
    extern volatile unsigned long   lastTimeCameraUsed;
    extern volatile bool            pause;
    extern volatile bool            isPaused;
    extern volatile float           framesPerSecond;
    extern volatile int             frameCount;
    extern volatile int64_t         lastTimeFramesCounted;
    extern bool                     cameraInitialized;
}