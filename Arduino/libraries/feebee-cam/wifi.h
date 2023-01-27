#pragma once
#include <DNSServer.h>
#include <WiFi.h>
#include <bee-fish.h>

namespace FeebeeCam {

    bool initializeWiFi();
    bool initializeDNSServer(IPAddress ipAddress);
    bool deinitializeDNSServer();


    bool connectToUserSSID(); // Used to test setup
    bool connectToDefaultSSID();

    extern bool isConnectedToInternet;
    extern bool isConnectedToESPAccessPoint;

    BeeFishBString::BString getURL(int port = -1);

    extern const IPAddress softAPIP;
 
    extern DNSServer* dnsServer;

}