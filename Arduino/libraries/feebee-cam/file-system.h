#include "FS.h"
#include <SPIFFS.h>
#include <map>
#include <WiFi.h>
#include "web-request.h"

namespace FeebeeCam {

    bool initializeFileSystem();
 
    void downloadRequiredFiles();

    bool downloadFile(const BString& source, const BString& destination);

    extern bool downloadWhenReady;
}