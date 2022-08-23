#pragma once
#include <bee-fish.h>
#include "esp_camera.h"
#include "camera-pins.h"
#include "web-server.h"

namespace FeebeeCam {
    
    bool initializeCamera(size_t frameBufferCount = FRAME_BUFFER_COUNT);
    bool onCamera(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onCapture(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    camera_fb_t* getImage();
    bool uploadImage();
    
    double getFrameRate();
            
    extern volatile bool            stop;
    extern volatile bool            isCameraRunning;
    extern volatile bool            pause;
    extern volatile bool            isPaused;
    extern volatile float           framesPerSecond;
    extern volatile int             frameCount;
    extern volatile int64_t         lastTimeFramesCounted;
    extern bool                     cameraInitialized;

}

