#ifndef WEB_SERVER2
#define WEB_SERVER2

#include <Arduino.h>
#include <mutex>
#include "web-server-base.h"
#include "web-client.h"

namespace FeebeeCam {

    bool initializeWebServer();
    bool onLight(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

    extern WebServer* webServer;
    extern WebServer* webServerCamera;

}

#endif