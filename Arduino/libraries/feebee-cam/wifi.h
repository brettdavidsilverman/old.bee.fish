#pragma once
#include <WiFi.h>
#include <bee-fish.h>

namespace FeebeeCam {

    bool initializeWiFi();

    bool connectToUserSSID(); // Used to test setup
    
    extern bool connectedToInternet;
    extern bool connectedToAccessPoint;
    
    BeeFishBString::BString getURL();
}