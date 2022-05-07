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
            (*this)["frameSize"] = (double)FRAMESIZE_CIF;
            (*this)["quality"] = 10;
        }

        void initialize() {
            Serial.println("Getting camera settings");

            BeeFishBScript::Object settings;

            if (BeeFishStorage::getItem("/beehive/", "settings", settings)) {
                // Add the retrieved settings to this object
                apply(settings);
                // Apply setttings to camera
                applySettings();    
            }
            else
                Serial.println("Error getting camera settings");

        }

        void applySettings() {
            
            Serial.println("Applying camera settings");

            sensor_t *sensor = esp_camera_sensor_get();

            sensor->set_framesize(sensor, (framesize_t)(BeeFishBScript::Number)(*this)["frameSize"]);

            sensor->set_quality(sensor, (BeeFishBScript::Number)(*this)["quality"]);

        /*
        sensor->set_gainceiling(sensor, (gainceiling_t)(settings["gainCeiling"]));
        sensor->set_quality(sensor, settings["quality"]);
        sensor->set_brightness(sensor, settings["brightness"]);
        sensor->set_contrast(sensor, settings["contrast"]);
        sensor->set_saturation(sensor, settings["saturation"]);
        */
        }

        void save() {
            Serial.println("Saving camera settings");
            BeeFishStorage::setItem("/beehive/", "settings", *this);
        }

    };

    extern Settings settings;
   
}