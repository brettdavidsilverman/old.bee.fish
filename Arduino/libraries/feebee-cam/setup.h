#ifndef FEEBEE_CAM__SETUP_H
#define FEEBEE_CAM__SETUP_H
#include <bee-fish.h>
#include "camera.h"
#include "web-client.h"
#include "rtc-bee.h"
#include "FS.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <SPIFFS.h>

namespace FeebeeCam {

    bool initializeSetup();

    bool onSettings(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onStatus(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onRestart(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

    extern BeeFishBScript::Object status;
    
    using namespace BeeFishBString;
    using namespace fs;

    class Setup : public BeeFishBScript::Object {
    public:
        BString _label;
        BString _ssid;
        BString _password;
        BString _secretHash;
        BString _beehiveVersion;
        BString _host;
        BString _timeZone;
        BString _timeZoneLabel;
        int     _frameSize;
        int     _gainCeiling;
        int     _quality;
        int     _brightness;
        int     _contrast;
        int     _saturation;
//        bool    _isRTCSetup;
        bool    _isSetup;
    public:
        
        virtual bool load() {
            using namespace BeeFishBScript;

            cerr << "Loading setup from setup.json" << endl;

            File file = SPIFFS.open("/setup.json", FILE_READ);

            BeeFishJSON::JSON json;
            BeeFishBScript::BScriptParser parser(json);

            Data data = Data::create();

            size_t bytesToRead = data.size();
            size_t totalBytes = file.size();
            size_t bytesRead = 0;

            while (bytesRead < totalBytes) {
                if (bytesToRead + bytesRead > totalBytes)
                    bytesToRead = totalBytes - bytesRead;

                bytesRead += 
                    file.readBytes((char*)data._readWrite, bytesToRead);
                    
                parser.read(data);

                if (parser.result() == false) {
                    file.close();
                    return false;
                }
            }

            file.close();

            if (parser.result() == true) {
                apply(parser.json());
            }
            else {
                return false;
            }

            _label          = (*this)["label"];
            _ssid           = (*this)["ssid"];
            _password       = (*this)["password"];
            _secretHash     = (*this)["secretHash"];
            _beehiveVersion = (*this)["beehiveVersion"];
            _host           = (*this)["host"];
            _timeZone       = (*this)["timeZone"];
            _timeZoneLabel  = (*this)["timeZoneLabel"];
            _frameSize      = (Number)(*this)["frameSize"];
            _gainCeiling    = (Number)(*this)["gainCeiling"];
            _quality        = (Number)(*this)["quality"];
            _brightness     = (Number)(*this)["brightness"];
            _contrast       = (Number)(*this)["contrast"];
            _saturation     = (Number)(*this)["saturation"];
            _isSetup        = (Boolean)(*this)["isSetup"];

            return true;

        }
        
        Setup() {

        }

        virtual bool inititalize() {

            using namespace BeeFishBScript;

            bool success = false;
            
            if (SPIFFS.exists("/setup.json"))
                success = load();
            else
                success = reset();

            if (success)
                std::cerr << "Setup initialized" << std::endl;
            else
                std::cerr << "Error initializing setup" << std::endl;

            return success;
        }



        bool save() {
            
            using namespace BeeFishBScript;
            std::cerr << "Saving file to flash" << std::endl;
            
            clear();

            (*this)["label"]          = _label;
            (*this)["ssid"]           = _ssid;
            (*this)["password"]       = _password;
            (*this)["secretHash"]     = _secretHash;
            (*this)["beehiveVersion"] = _beehiveVersion;
            
            (*this)["host"]          = _host;
            (*this)["timeZone"]      = _timeZone;
            (*this)["timeZoneLabel"] = _timeZoneLabel;
            (*this)["frameSize"]     = (Number)_frameSize;
            (*this)["gainCeiling"]   = (Number)_gainCeiling;
            (*this)["quality"]       = (Number)_quality;
            (*this)["brightness"]    = (Number)_brightness;
            (*this)["contrast"]      = (Number)_contrast;
            (*this)["saturation"]    = (Number)_quality;
            (*this)["isSetup"]       = (Boolean)_isSetup;


            if (SPIFFS.exists("/setup.json"))
                SPIFFS.remove("/setup.json");

            File file = SPIFFS.open("/setup.json", FILE_WRITE);

            std::string string = str();
            file.write((const uint8_t*)string.data(), string.size());

            file.close();

            return true;
        }
            
        virtual bool reset() {

            using namespace BeeFishBScript;

            cerr << "Using default setup" << endl;

            // Initial setup
            (*this)["label"]          = _label          = MY_LABEL;
            (*this)["ssid"]           = _ssid           = MY_SSID;
            (*this)["password"]       = _password       = MY_PASSWORD;
            (*this)["secretHash"]     = _secretHash     = PUBLIC_SECRET_HASH;
            (*this)["beehiveVersion"] = _beehiveVersion = BEEHIVE_VERSION;

            (*this)["host"]          = _host          = HOST;
            (*this)["timeZone"]      = _timeZone      = MY_TIMEZONE;
            (*this)["timeZoneLabel"] = _timeZoneLabel = MY_TIMEZONE_LABEL;
            (*this)["frameSize"]     = _frameSize     = (Number)FRAMESIZE_CIF;
            (*this)["gainCeiling"]   = _gainCeiling   = (Number)255.0;
            (*this)["quality"]       = _quality       = (Number)10.0;
            (*this)["brightness"]    = _brightness    = (Number)0.0;
            (*this)["contrast"]      = _contrast      = (Number)0.0;
            (*this)["saturation"]    = _saturation    = (Number)0.0;
            (*this)["isSetup"]       = _isSetup       = (Boolean)false;

            return true;
        }

    
        void applyToCamera() {
            using namespace BeeFishBScript;
            
            Serial.println("Applying camera setup");

            sensor_t *sensor = esp_camera_sensor_get();

            if (sensor) {

                sensor->set_framesize(sensor, (framesize_t)(Number)(*this)["frameSize"]);

                sensor->set_gainceiling(sensor, (gainceiling_t)(Number)(*this)["gainCeiling"]);

                sensor->set_quality(sensor, (int)(Number)(*this)["quality"]);

                sensor->set_brightness(sensor, (int)(Number)(*this)["brightness"]);

                sensor->set_contrast(sensor, (int)(Number)(*this)["contrast"]);

                sensor->set_saturation(sensor, (int)(Number)(*this)["saturation"]);

                // Turn the camera the right way round
                sensor->set_vflip(sensor, (int)1);
                sensor->set_hmirror(sensor, (int)1);

                std::cerr << "Setup applied to camera" << std::endl;
            }
            else
                std::cerr << "Error applying setup to camera" << std::endl;

        }

    };

    extern Setup* _setup;

 
}

#endif