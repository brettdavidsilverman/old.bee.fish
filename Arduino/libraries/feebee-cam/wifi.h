#pragma once
#include <DNSServer.h>
#include <WiFi.h>
#include <bee-fish.h>

namespace FeebeeCam {

    bool initializeWiFi();
    bool initializeDNSServer();

    bool connectToUserSSID(); // Used to test setup
    bool connectToLocalSSID();

    extern bool connectedToInternet;
    extern bool connectedToAccessPoint;
    
    BeeFishBString::BString getURL();

    extern const IPAddress softAPIP;
 
    extern DNSServer* dnsServer;

}