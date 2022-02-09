#include "web-server-helpers.h"

namespace FeebeeCam {
    
    void initializeCamera();

    void onCameraGet(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res);

    extern bool     stop;
    extern bool     isRunning;
    extern float    framesPerSecond;
    extern int      frameCount;
    extern int64_t  lastTimeFramesCounted;
}