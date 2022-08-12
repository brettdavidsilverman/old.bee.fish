#pragma once
#include <bee-fish.h>
#include "esp_camera.h"
#include "camera-pins.h"
#include "web-server.h"

namespace FeebeeCam {
    
    bool initializeCamera(int frameBufferCount = FRAME_BUFFER_COUNT);
    bool onCamera(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onCapture(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    BeeFishBString::Data* getImage();

    double getFrameRate();
            
    extern volatile bool            stop;
    extern volatile bool            isCameraRunning;
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
        volatile bool _isRunning;
        volatile bool _isStopped;
        TaskHandle_t _cameraLoopHandle;
    public:
        
        FrameBufferQueue(int queueSize) :
            _queueSize(queueSize) 
        {
            _isRunning = false;
            _isStopped = true;
            _cameraLoopHandle = NULL;
        }

        virtual ~FrameBufferQueue() {
            stop();
        }

        bool start() {
            
            if(_isRunning)
                return true;

            _cameraLoopHandle = NULL;

            xTaskCreatePinnedToCore(
                FrameBufferQueue::cameraQueue,        // Task function. 
                "cameraQueue",      // String with name of task. 
                2048,               // Stack size in bytes. 
                this,               // Parameter passed as input of the task 
                3,                  // Priority of the task. 
                &_cameraLoopHandle,           // Task handle
                1                   // Pinned to core 
            );

            if (_cameraLoopHandle == NULL) {
                std::cerr << "Error starting camera queue loop task" << std::endl;
                return false;
            }

            return true;
        }

        void stop() {
            if (!_isRunning)
                return;

            _isStopped = false;
            _isRunning = false;

            while (!_isStopped) {
                delay(10);
            }

            std::cerr << "Frame buffer queue loop stopped" << std::endl;

            while (size()) {
                camera_fb_t* frameBuffer = front();
                esp_camera_fb_return(frameBuffer);
                pop();
            }
        }

        static void cameraQueue(void* pointer) {

            FrameBufferQueue* _this = (FrameBufferQueue*)pointer;

            _this->_isRunning = true;
            _this->_isStopped = false;

            std::cerr << "Frame buffer queue loop started" << std::endl;

            while (_this->_isRunning) {

                camera_fb_t* frameBuffer = esp_camera_fb_get();
                
                if (frameBuffer)
                    _this->push_back(frameBuffer);
                else
                    cerr << "Frame buffer capture error" << std::endl;

                delay(1);
            }

            _this->_isStopped = true;

            vTaskDelete(_this->_cameraLoopHandle);
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
            
            // Clear from queue
            while (size()) {
                frameBuffer = front();
                esp_camera_fb_return(frameBuffer);
                pop();
            }

            // Clear from camera
            for (int i = 0; i < _queueSize; ++i) {
                frameBuffer = esp_camera_fb_get();
                if (frameBuffer)
                    esp_camera_fb_return(frameBuffer);
            }         

            frameBuffer = esp_camera_fb_get();

            return frameBuffer;

        }

    };


}

