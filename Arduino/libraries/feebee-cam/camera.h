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

    class FrameBufferQueue : private std::queue<camera_fb_t *> {
    protected:
        std::mutex _mutex;
        const int _queueSize;
    public:
        
        FrameBufferQueue(int queueSize) :
            _queueSize(queueSize)
        {

        }

        void push_back(camera_fb_t * frameBuffer) {
            const std::lock_guard<std::mutex> lock(_mutex);

            while (size() >= _queueSize) {
                camera_fb_t* discarded = front();
                pop();
                esp_camera_fb_return(discarded);
            }

            std::queue<camera_fb_t *>::push(frameBuffer);
        }

        camera_fb_t* pop_front() {
            
            while (size() == 0) {
                delay(10);
            }

            const std::lock_guard<std::mutex> lock(_mutex);

            camera_fb_t* frameBuffer = front();

            std::queue<camera_fb_t*>::pop();

            return frameBuffer;
        }

        camera_fb_t* flush() {
            const std::lock_guard<std::mutex> lock(_mutex);
            camera_fb_t* frameBuffer;
            while (size()) {
                frameBuffer = front();
                esp_camera_fb_return(frameBuffer);
                pop();
            }
            frameBuffer = esp_camera_fb_get();
            return frameBuffer;
        }

    };


}

