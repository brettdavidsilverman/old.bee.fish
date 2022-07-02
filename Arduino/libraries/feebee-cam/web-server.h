#pragma once
#include "wifi-web-server.h"

namespace FeebeeCam {

    bool initializeWebServer();

    // Two Web Servers, one on each core
    extern BeeFishWebServer::WebServer* webServer;

}

