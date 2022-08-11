#pragma once
#include <WiFi.h>
#include <bee-fish.h>

namespace FeebeeCam {

    void initializeWiFi();
 
    extern bool connectedToInternet;
    extern bool connectedToAccessPoint;
    
    BeeFishBString::BString getURL();
}