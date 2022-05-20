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
            reset();
        }

        void reset() {
            // Initial settings
            (*this)["host"] = HOST;
            (*this)["frameSize"] = (double)FRAMESIZE_CIF;
            (*this)["gainCeiling"] = 255;
            (*this)["quality"] = 10;
            (*this)["brightness"] = 0;
            (*this)["contrast"] = 0;
            (*this)["saturation"] = 0;
            (*this)["wakeup"] = true;
            (*this)["awake"] = false;
        }

        void initialize() {
            Serial.println("Getting camera settings");

            BeeFishStorage storage;

            BeeFishBScript::Variable& value = 
                storage.getItem("/beehive/", "settings");

            if (value == nullptr)
                save();
            else if (value.type() == BeeFishJSON::Type::OBJECT)
                apply((BeeFishBScript::ObjectPointer)value);
            else
                Serial.println("Error retrieving camera settings");

            // Apply setttings to camera
            applyToCamera();    
        }

        void applyToCamera() {
            
            Serial.println("Applying camera settings");

            sensor_t *sensor = esp_camera_sensor_get();

            sensor->set_framesize(sensor, (framesize_t)(BeeFishBScript::Number)(*this)["frameSize"]);

            sensor->set_gainceiling(sensor, (gainceiling_t)(BeeFishBScript::Number)(*this)["gainCeiling"]);

            sensor->set_quality(sensor, (int)(BeeFishBScript::Number)(*this)["quality"]);

            sensor->set_brightness(sensor, (int)(BeeFishBScript::Number)(*this)["brightness"]);

            sensor->set_contrast(sensor, (int)(BeeFishBScript::Number)(*this)["contrast"]);

            sensor->set_saturation(sensor, (int)(BeeFishBScript::Number)(*this)["saturation"]);

        /*
        sensor->set_gainceiling(sensor, (gainceiling_t)(settings["gainCeiling"]));
        sensor->set_quality(sensor, settings["quality"]);
        sensor->set_brightness(sensor, settings["brightness"]);
        sensor->set_contrast(sensor, settings["contrast"]);
        sensor->set_saturation(sensor, settings["saturation"]);
        */
        }

        bool save() {
            Serial.println("Saving camera settings");
            BeeFishStorage storage;
            bool saved = storage.setItem("/beehive/", "settings", *this);
            if (saved)
                Serial.println("Saved");
            else
                Serial.println("Error saving settings");
            return saved;
        }

    };

    bool initializeSettings();

    extern Settings settings;
   
}