#include "FS.h"
#include "SPIFFS.h"
#include <map>
#include <WiFi.h>
#include <web-request.h>

namespace FeebeeCam {

    void initializeFileSystem();
 
    void downloadRequiredFiles();

    extern bool downloadWhenReady;
}