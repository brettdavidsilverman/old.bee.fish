#pragma once
#include "wifi-web-server.h"

namespace FeebeeCam {

    void initializeWebServer();

    // Two Web Servers, one on each core
    extern WiFiWebServer* webServer0;
    extern WiFiWebServer* webServer1;

}

