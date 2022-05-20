#include "settings.h"

namespace FeebeeCam {

    Settings settings;

    bool initializeSettings() {


      if (_setup._secretHash.length()) {

         if (BeeFishWebRequest::logon(_setup._secretHash)) {

            Serial.println("Synchronizing settings");
      
            settings.initialize();

            settings["awake"] = true;
            settings["ssid"] = _setup._ssid;
            settings["label"] = _setup._label;
            settings["url"] = "http://" + BString(WiFi.localIP().toString().c_str()) + "/";

            return settings.save();
         }

      }

      return false;

    }
}