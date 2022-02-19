#include <bee-fish.h>
#include <wifi-web-server.h>

namespace FeebeeCam {
    
    void initializeCamera();

    bool onCameraGet(BeeFishWeb::WebRequest& request, WiFiClient& client);

    extern bool     stop;
    extern bool     isRunning;
    extern float    framesPerSecond;
    extern int      frameCount;
    extern int64_t  lastTimeFramesCounted;
}