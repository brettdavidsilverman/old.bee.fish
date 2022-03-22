#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include "wifi-web-server.h"
#include "file-system.h"

namespace FeebeeCam {

   bool onFileServer(BeeFishWeb::WebRequest& request, WiFiClient& client);
   
}