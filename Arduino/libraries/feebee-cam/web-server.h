#ifndef WEB_SERVER2
#define WEB_SERVER2

#include <Arduino.h>
#include <mutex>
#include "web-server-base.h"

namespace FeebeeCam {

    bool initializeWebServers();

    extern WebServer* webServer;
    extern WebServer* webServerCamera;

}

#endif