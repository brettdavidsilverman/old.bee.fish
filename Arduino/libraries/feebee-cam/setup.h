#ifndef FEEBEE_CAM__SETUP_H
#define FEEBEE_CAM__SETUP_H
#include <bee-fish.h>
#include "camera.h"
#include "FS.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <SPIFFS.h>

namespace FeebeeCam {

    class WebClient;
    
    bool initializeSetup();

    bool onSetupBeehive(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onRestart(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onRedirect(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);
    bool onGenerate204(const BeeFishBString::BString& path, FeebeeCam::WebClient* client);

    using namespace BeeFishBString;
    using namespace fs;


    class Setup : public BeeFishBScript::Object {
    public:
        const BString _fileName = "/setup.json";

        BeeFishBScript::String  _label            = MY_LABEL;
        BeeFishBScript::String  _ssid             = MY_SSID;
        BeeFishBScript::String  _password         = MY_PASSWORD;
        BeeFishBScript::String  _secretHash       = PUBLIC_SECRET_HASH;
        BeeFishBScript::String  _version          = BEEHIVE_VERSION;
        BeeFishBScript::String  _host             = HOST;
        BeeFishBScript::String  _timeZone         = MY_TIMEZONE;
        BeeFishBScript::String  _timeZoneLabel    = MY_TIMEZONE_LABEL;
        BeeFishBScript::Number  _wakeupEvery      = WAKEUP_EVERY_SECONDS;
        BeeFishBScript::Number  _takePictureEvery = TAKE_PICTURE_EVERY;
        BeeFishBScript::Number  _frameSize        = DEFAULT_FRAMESIZE;
        BeeFishBScript::Number  _gainCeiling      = DEFAULT_GAIN_CEILING;
        BeeFishBScript::Number  _quality          = DEFAULT_QUALITY;
        BeeFishBScript::Number  _brightness       = DEFAULT_BRIGHTNESS;
        BeeFishBScript::Number  _contrast         = DEFAULT_CONTRAST;
        BeeFishBScript::Number  _saturation       = DEFAULT_SATURATION;
        
        BeeFishBScript::Boolean _isSetup          = false;


    public:

        Setup() {

        }

        virtual bool load() {
            using namespace BeeFishBScript;

            loadObjectFromFileSystem();

            if (contains("version"))
                _version = (*this)["version"];

            if (contains("label"))
                _label = (*this)["label"];

            if (contains("ssid"))
                _ssid = (*this)["ssid"];

            if (contains("password"))
                _password = (*this)["password"];

            if (contains("secretHash"))
                _secretHash = (*this)["secretHash"];

            if (contains("wakeupEvery"))
                _wakeupEvery = (*this)["wakeupEvery"];

            if (contains("takePictureEvery"))
                _takePictureEvery = (*this)["takePictureEvery"];

            if (contains("host"))
                _host = (*this)["host"];

            if (contains("timeZone"))
                _timeZone = (*this)["timeZone"];

            if (contains("timeZoneLabel"))
                _timeZoneLabel = (*this)["timeZoneLabel"];

            if (contains("frameSize"))
                _frameSize = (*this)["frameSize"];

            if (contains("gainCeiling"))
                _gainCeiling = (*this)["gainCeiling"];
            
            if (contains("quality"))
                _quality = (*this)["quality"];

            if (contains("brightness"))
                _brightness = (*this)["brightness"];

            if (contains("contrast"))
                _contrast = (*this)["contrast"];

            if (contains("saturation"))
                _saturation =  (*this)["saturation"];

            if (contains("isSetup"))
                _isSetup = (*this)["isSetup"];

            clearSecretInformation();

            return true;

        }
        
        virtual bool loadObjectFromFileSystem() {
    
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
            
            const std::string fileName = _fileName.str();

            BeeFishBScript::Object copy;

            assign(copy, true);

            if (SPIFFS.exists(fileName.c_str()))
                SPIFFS.remove(fileName.c_str());

            File file = SPIFFS.open(fileName.c_str(), FILE_WRITE);

            std::string string = copy.str();

            file.write((const uint8_t*)string.data(), string.size());

            file.close();

            clearSecretInformation();

            return true;
        }

        friend ostream& operator << (ostream& out, const Setup& setup) {
            std::cerr << "Setup::operator << " << std::endl;
            BeeFishBScript::Object copy;
            setup.assign(copy, true);
            out << copy;
            return out;
        }

        virtual void assign(BeeFishBScript::Object& copy, bool includeSecretInformation) const {
            
            copy["version"]        = _version;
            copy["host"]           = _host;
            
            copy["label"]          = _label;
            copy["ssid"]           = _ssid;

            if (includeSecretInformation) {
                copy["password"]       = _password;
                copy["secretHash"]     = _secretHash;
            }
            
            copy["timeZone"]         = _timeZone;
            copy["timeZoneLabel"]    = _timeZoneLabel;
            copy["wakeupEvery"]      = _wakeupEvery;
            copy["takePictureEvery"] = _takePictureEvery;
            copy["frameSize"]        = _frameSize;
            copy["gainCeiling"]      = _gainCeiling;
            copy["quality"]          = _quality;
            copy["brightness"]       = _brightness;
            copy["contrast"]         = _contrast;
            copy["saturation"]       = _saturation;

            copy["isSetup"]          = _isSetup;

        }

        virtual void clearSecretInformation() {

            if (contains("password"))
                erase("password");

            if (contains("secretHash"))
                erase("secretHash");

        }
            
        virtual bool reset() {

            using namespace BeeFishBScript;

            cerr << "Using default setup" << endl;

            // Reset to initial camera settings

            _frameSize      = DEFAULT_FRAMESIZE;
            _gainCeiling    = DEFAULT_GAIN_CEILING;
            _quality        = DEFAULT_QUALITY;
            _brightness     = DEFAULT_BRIGHTNESS;
            _contrast       = DEFAULT_CONTRAST;
            _saturation     = DEFAULT_SATURATION;

            save();

//            applyToCamera();

            return true;
        }

    
        void applyToCamera() {
            using namespace BeeFishBScript;
            
            std::cerr << "Applying camera setup..." << std::flush;

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

                std::cerr << "Ok" << std::endl;
            }
            else
                std::cerr << "Error" << std::endl;

        }

    };

    extern Setup* _setup;

 
}

#endif