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
    bool uploadImage(BeeFishId::Id& id);
    bool stopCamera();
    bool pauseCamera();
    bool resumeCamera();
    bool flushFrameBuffer();
    void resetCameraWatchDogTimer();

    double getFrameRate();
            
    extern bool    stop;
    extern bool    isCameraRunning;
    extern bool    pause;
    extern bool    isCameraPaused;
    extern float   framesPerSecond;
    extern int     frameCount;
    extern int64_t lastTimeFramesCounted;
    extern bool    isCameraInitialized;
    extern int64_t cameraWatchDogTimer;
}

