#pragma once
#include "FS.h"
#include "SPIFFS.h"
#include <bee-fish.h>
#include "camera.h"
#include "web-storage.h"

namespace FeebeeCam {

    bool initializeSerial();
    bool checkCommandLine();
    bool handleCommandLine();
    
}
