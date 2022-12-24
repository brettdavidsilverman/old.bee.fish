#ifndef FEEBEE_CAM__SETUP_H
#define FEEBEE_CAM__SETUP_H
#include <bee-fish.h>
#include "camera.h"
#include "web-client.h"
#include "FS.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <SPIFFS.h>

namespace FeebeeCam {

    bool initializeSetup();

    bool onSetupBeehive(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onRestart(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onRedirect(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onGenerate204(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

    using namespace BeeFishBString;
    using namespace fs;

    class Setup : public BeeFishBScript::Object {
    public:
        const BeeFishBScript::Number  _defaultFrameSize   = FRAMESIZE_CIF;
        const BeeFishBScript::Number  _defaultGainCeiling = 255.0;
        const BeeFishBScript::Number  _defaultQuality     = 10.0;
        const BeeFishBScript::Number  _defaultBrightness  = 0.0;
        const BeeFishBScript::Number  _defaultContrast    = 0.0;
        const BeeFishBScript::Number  _defaultSaturation  = 0.0;

        const BString _fileName = "/setup.json";

        BString _label;
        BString _ssid;
        BString _password;
        BString _secretHash;
        BString _beehiveVersion;
        BString _host;
        BString _timeZone;
        BString _timeZoneLabel;
        
        int     _frameSize   = _defaultFrameSize;
        int     _gainCeiling = _defaultGainCeiling;
        int     _quality     = _defaultQuality;
        int     _brightness  = _defaultBrightness;
        int     _contrast    = _defaultContrast;
        int     _saturation  =_defaultSaturation;
        bool    _isSetup     = false;


    public:
        
        virtual bool load() {
            using namespace BeeFishBScript;

            loadFromFileSystem();

            _label          = contains("label") ?
                                (BString&)(*this)["label"] :
                                BString("Bloody Bees");

            _ssid           = contains("ssid") ?
                                (BString&)(*this)["ssid"] :
                                BString("My wifi name");

            _password       = contains("password") ?
                                (BString&)(*this)["password"] :
                                BString("password");

            _secretHash     = contains("secretHash") ?
                                (BString&)(*this)["secretHash"] :
                                BString("");

            _beehiveVersion = contains("beehiveVersion") ?
                                (BString&)(*this)["beehiveVersion"] :
                                BString(BEEHIVE_VERSION);

            _host           = contains("host") ?
                                (BString&)(*this)["host"] :
                                BString(HOST);

            _timeZone       = contains("timeZone") ?
                                (BString&)(*this)["timeZone"] :
                                BString(MY_TIMEZONE);

            _timeZoneLabel  = contains("timeZoneLabel") ?
                                (BString&)(*this)["timeZoneLabel"] :
                                BString("Australia/Brisbane");

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
    
            std::string fileName = _fileName.str();

            if (SPIFFS.exists(fileName.c_str()))
            {
                cerr << "Loading setup from setup.json" << endl;
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
            else {
                cerr << "Loading defaults" << endl;
                reset();
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

//            applyToCamera();

            return true;
        }

    
        void applyToCamera() {
            using namespace BeeFishBScript;
            
            Serial.println("Applying camera setup");

            sensor_t *sensor = esp_camera_sensor_get();

            if (sensor) {

                sensor->set_framesize(sensor, (framesize_t)_frameSize);

                sensor->set_gainceiling(sensor, (gainceiling_t)_gainCeiling);

                sensor->set_quality(sensor, (int)_quality);

                sensor->set_brightness(sensor, (int)_brightness);

                sensor->set_contrast(sensor, (int)_contrast);

                sensor->set_saturation(sensor, (int)_saturation);

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