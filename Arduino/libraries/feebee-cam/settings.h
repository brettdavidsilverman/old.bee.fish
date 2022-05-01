#pragma once
#include "FS.h"
#include "SPIFFS.h"
#include <bee-fish.h>
#include "camera.h"
#include "web-storage.h"

namespace FeebeeCam {

    class Settings : public BeeFishBScript::Object {
    private:

    public:
        Settings() {

        }

        void initialize() {
            Serial.println("Getting camera settings");
            if (!BeeFishStorage::getItem("/beehive/", "settings", *this)) {
                Serial.println("Setting default camera settings");
                (*this)["frameSize"] = (int)FRAMESIZE_CIF;
                (*this)["quality"] = 10;
            }
        }

        void save() {
            Serial.println("Saving camera settings");
            BeeFishStorage::setItem("/beehive/", "settings", *this);
        }

    };

    extern Settings settings;

    bool onSettings(BeeFishWeb::WebRequest& request, WiFiClient& client);
    
}