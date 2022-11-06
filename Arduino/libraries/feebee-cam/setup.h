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

    bool onSetupBeehive(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onRestart(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

    using namespace BeeFishBString;
    using namespace fs;

    class Setup : public BeeFishBScript::Object {
    public:
        const BString _fileName = "/setup.json";
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
        bool    _isRTCSetup;
        bool    _isSetup;

        const BeeFishBScript::Number  _defaultFrameSize   = FRAMESIZE_CIF;
        const BeeFishBScript::Number  _defaultGainCeiling = 255.0;
        const BeeFishBScript::Number  _defaultQuality     = 10.0;
        const BeeFishBScript::Number  _defaultBrightness  = 0.0;
        const BeeFishBScript::Number  _defaultContrast    = 0.0;
        const BeeFishBScript::Number  _defaultSaturation  = 0.0;

    public:
        
        virtual bool load(bool defaults = false) {
            using namespace BeeFishBScript;

            if (defaults) {
                cerr << "Loading defaults" << endl;
                clear();
            }
            else {
                if (!loadFromFileSystem())
                    clear();
            }

            _label          = contains("label") ?
                                (*this)["label"] :
                                "Bloody Bees";

            _ssid           = contains("ssid") ?
                                (*this)["ssid"] :
                                "My wifi name";

            _password       = contains("password") ?
                                (*this)["password"] :
                                "My Wifi password";

            _secretHash     = contains("secretHash") ?
                                (*this)["secretHash"] :
                                "";

            _beehiveVersion = contains("beehiveVersion") ?
                                (*this)["beehiveVersion"] :
                                BEEHIVE_VERSION;

            _host           = contains("host") ?
                                (*this)["host"] :
                                HOST;

            _timeZone       = contains("timeZone") ?
                                (*this)["timeZone"] :
                                MY_TIMEZONE;

            _timeZoneLabel  = contains("timeZoneLabel") ?
                                (*this)["timeZoneLabel"] :
                                "Australia/Brisbane";

            _frameSize      = contains("frameSize") ?
                                (Number)(*this)["frameSize"] :
                                _defaultFrameSize;

            _gainCeiling    = contains("gainCeiling") ?
                                (Number)(*this)["gainCeiling"] :
                                _defaultGainCeiling;

            
            _quality        = contains("quality") ?
                                (Number)(*this)["quality"] :
                                _defaultQuality;

            _brightness     = contains("brightness") ?
                                (Number)(*this)["brightness"] :
                                _defaultBrightness;

            _contrast       = contains("contrast") ?
                                (Number)(*this)["contrast"] :
                                _defaultContrast;

            _saturation     = contains("saturation") ? 
                                (Number)(*this)["saturation"] :
                                _defaultSaturation;

            _isRTCSetup     = contains("isRTCSetup") ?
                                (Boolean)(*this)["isRTCSetup"] :
                                false;
            ;
            _isSetup        = contains("isSetup") ?
                                (Boolean)(*this)["isSetup"] :
                                false;

            clearSecretInformation();

            return true;

        }
        
        Setup() {

        }

        virtual bool loadFromFileSystem() {
            cerr << "Loading setup from setup.json" << endl;
    
            std::string fileName = _fileName.str();

            if (SPIFFS.exists(fileName.c_str()))
            {
                File file = SPIFFS.open(fileName.c_str(), FILE_READ);

                BeeFishJSON::JSON json;
                BeeFishBScript::BScriptParser parser(json);

                Data data = Data::create();

                size_t bytesToRead = data.size();
                size_t totalBytes = file.size();
                size_t totalBytesRead = 0;

                while (totalBytesRead < totalBytes) {
                    if (bytesToRead +totalBytesRead > totalBytes)
                        bytesToRead = totalBytes - totalBytesRead;

                    size_t bytesRead = 
                        file.readBytes((char*)data._readWrite, bytesToRead);
                        
                    totalBytesRead += bytesRead;
                    parser.read(data, bytesRead);

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

                return true;
            }

            return false;

        }

        virtual bool inititalize() {

            using namespace BeeFishBScript;

            bool success = false;
            
            success = load();

            if (success)
                std::cerr << "Setup initialized" << std::endl;
            else
                std::cerr << "Error initializing setup" << std::endl;

            return success;
        }



        bool save() {
            
            using namespace BeeFishBScript;
            std::cerr << "Saving setup.json to flash" << std::endl;
            
            clear();

            const std::string fileName = _fileName.str();

            assign(true);


            if (SPIFFS.exists(fileName.c_str()))
                SPIFFS.remove(fileName.c_str());

            File file = SPIFFS.open(fileName.c_str(), FILE_WRITE);

            std::string string = str();
            file.write((const uint8_t*)string.data(), string.size());

            file.close();

            clearSecretInformation();

            return true;
        }

        virtual void assign(bool includeSecretInformation) {
            using namespace BeeFishBScript;
            
            (*this)["label"]          = _label;
            (*this)["ssid"]           = _ssid;
            if (includeSecretInformation) {
                (*this)["password"]       = _password;
                (*this)["secretHash"]     = _secretHash;
            }
            (*this)["beehiveVersion"] = _beehiveVersion;
            
            (*this)["host"]          = _host;
            (*this)["timeZone"]      = _timeZone;
            (*this)["timeZoneLabel"] = _timeZoneLabel;
            (*this)["frameSize"]     = (Number)_frameSize;
            (*this)["gainCeiling"]   = (Number)_gainCeiling;
            (*this)["quality"]       = (Number)_quality;
            (*this)["brightness"]    = (Number)_brightness;
            (*this)["contrast"]      = (Number)_contrast;
            (*this)["saturation"]    = (Number)_saturation;
            (*this)["isRTCSetup"]    = (Boolean)_isRTCSetup;
            (*this)["isSetup"]       = (Boolean)_isSetup;

        }

        virtual void clearSecretInformation() {
            (*this)["password"]   = undefined;
            (*this)["secretHash"] = undefined;
        }
            
        virtual bool reset() {

            using namespace BeeFishBScript;

            cerr << "Using default setup" << endl;

            // Reset to initial camera settings
            _frameSize      = _defaultFrameSize;
            _gainCeiling    = _defaultGainCeiling;
            _quality        =  _defaultQuality;
            _brightness     = _defaultBrightness;
            _contrast       = _defaultContrast;
            _saturation     = _defaultSaturation;

            save();

            applyToCamera();

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

                flushFrameBuffer();

                std::cerr << "Setup applied to camera" << std::endl;
            }
            else
                std::cerr << "Error applying setup to camera" << std::endl;

        }

    };

    extern Setup* _setup;

 
}

#endif