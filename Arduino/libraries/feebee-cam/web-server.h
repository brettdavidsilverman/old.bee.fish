#pragma once
#include <Arduino.h>
#include <bee-fish.h>

namespace FeebeeCam {

    bool initializeWebServer();

    // Two Web Servers, one on each core
    extern WebServer* webServer80;
    extern WebServer* webServer8080;

}

