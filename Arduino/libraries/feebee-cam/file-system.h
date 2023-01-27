#ifndef FEEBEECAM__FILE_SYSTEM_H
#define FEEBEECAM__FILE_SYSTEM_H

#include "FS.h"
#include <SPIFFS.h>
#include <map>
#include <WiFi.h>
#include "web-request.h"
#include "web-client.h"
#include "setup.h"

namespace FeebeeCam {

    bool initializeFileSystem();
 
    bool downloadFiles(bool force = false, bool downloadBinary = false);

    bool downloadFile(BString source, BString destination, bool print = false);

    bool onDownloadFiles(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onDownloadStatus(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    
    extern BeeFishBScript::Object downloadStatus;

}

#endif