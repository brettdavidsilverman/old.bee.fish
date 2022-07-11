#pragma once
#include <WiFi.h>

namespace FeebeeCam {

    void initializeWiFi();
 
    extern volatile bool downloadWhenReady;
    extern volatile bool connectedToInternet;
    extern volatile bool connectedToAccessPoint;
    
}