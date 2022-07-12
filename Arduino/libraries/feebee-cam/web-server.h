#pragma once
#include <Arduino.h>
#include <bee-fish.h>

namespace FeebeeCam {

    bool initializeWebServer();

    // Two Web Servers, one on each core
    extern BeeFishWebServer::WebServer* webServer80;
    extern BeeFishWebServer::WebServer* webServer8080;

}

