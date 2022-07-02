#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include "wifi-web-server.h"
#include "file-system.h"

namespace FeebeeCam {

   void serveFile(const BString& fileName, Print& client);

   bool onFileServer(const BeeFishBString::BString& path, BeeFishWebServer::WebClient* client);
   
   extern std::map<BeeFishBString::BString, BeeFishBString::BString> CONTENT_TYPES;
   extern std::map<BeeFishBString::BString, bool> CACHE_RULES;

}