#include "FS.h"
#include <SPIFFS.h>
#include <map>
#include <WiFi.h>
#include "web-request.h"

namespace FeebeeCam {

    bool initializeFileSystem();
 
    bool downloadRequiredFiles();

    bool downloadFile(const BString& source, const BString& destination, bool print = false);

    extern bool downloadWhenReady;
}