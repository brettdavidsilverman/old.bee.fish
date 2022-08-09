#ifndef WEB_SERVER2
#define WEB_SERVER2

#include <Arduino.h>
#include <mutex>
#include "web-server-base.h"
#include "web-client.h"

namespace FeebeeCam {

    bool initializeWebServer();

}

#endif