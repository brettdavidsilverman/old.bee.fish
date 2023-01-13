#pragma once
#include <FS.h>
#include <SPIFFS.h>
#include "file-system.h"

namespace FeebeeCam {

   bool serveFile(const BString& path,  FeebeeCam::WebClient* client);

   bool onFileServer(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
   bool onFileServerRedirect(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
   
   extern std::map<BeeFishBString::BString, BeeFishBString::BString> CONTENT_TYPES;
   extern std::map<BeeFishBString::BString, bool> CACHE_RULES;

}