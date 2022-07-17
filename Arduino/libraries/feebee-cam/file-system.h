#include "FS.h"
#include <SPIFFS.h>
#include <map>
#include <WiFi.h>
#include "web-request.h"

namespace FeebeeCam {

    bool initializeFileSystem();
 
    bool downloadRequiredFiles(bool force = false);

    bool downloadFile(BString source, BString destination, bool print = false);

}