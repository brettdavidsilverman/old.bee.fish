#include <file-server.h>
#include "setup.h"
#include "commands.h"
#include "wifi.h"
#include "camera.h"

namespace FeebeeCam {

    Setup* _setup = nullptr;
    BeeFishBScript::Object status;

    using namespace fs;

    bool initializeSetup() {
        
        std::cerr << "Initializing setup object" << std::endl;

        FeebeeCam::_setup = new FeebeeCam::Setup();

        return FeebeeCam::_setup->inititalize();
    }

    bool onSettings(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        

        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        sensor_t *sensor = esp_camera_sensor_get();

        BString message;
        bool isSetup = false;
        bool shouldSave = false;

        if (client->_webRequest.method() == "POST") {

            shouldSave = true;

            BeeFishBScript::Variable& json = client->_parser.json();
            BeeFishBScript::ObjectPointer input = 
                (BeeFishBScript::ObjectPointer)json;
                
            if (input->contains("label")) {
                isSetup = true;
                FeebeeCam::_setup->_label = (*input)["label"];
            }

            if (input->contains("ssid")) {
                isSetup = true;
                FeebeeCam::_setup->_ssid = (*input)["ssid"];
            }

            if (input->contains("password")) {
                isSetup = true;
                FeebeeCam::_setup->_password = (*input)["password"];
            }
            
            if (input->contains("secretHash")) {
                isSetup = true;
                FeebeeCam::_setup->_secretHash = (*input)["secretHash"];
            }

            if (input->contains("frameSize")) {
                int frameSize = (int)(*input)["frameSize"];
                sensor->set_framesize(sensor, (framesize_t)frameSize);
                FeebeeCam::_setup->_frameSize = frameSize;
                message = "Frame size set";
            }
            
            if (input->contains("gainCeiling")) {
                int gainCeiling = (int)(*input)["gainCeiling"];
                sensor->set_gainceiling(sensor, (gainceiling_t)gainCeiling);
                FeebeeCam::_setup->_gainCeiling = gainCeiling;
                message = "Gain ceiling set";
            }
            
            if (input->contains("quality")) {
                int quality = (int)(*input)["quality"];
                sensor->set_quality(sensor, quality);
                FeebeeCam::_setup->_quality = quality;
                message = "Quality set";
            }
            
            if (input->contains("brightness")) {
                int brightness = (int)(*input)["brightness"];
                sensor->set_brightness(sensor, brightness);
                FeebeeCam::_setup->_brightness = brightness;
                message = "Brightness set";
            }
            
            if (input->contains("contrast")) {
                int contrast = (int)(*input)["contrast"];
                sensor->set_contrast(sensor, contrast);
                FeebeeCam::_setup->_contrast = contrast;
                message = "Contrast set";
            }
            
            if (input->contains("saturation")) {
                int saturation = (int)(*input)["saturation"];
                sensor->set_saturation(sensor, saturation);
                FeebeeCam::_setup->_saturation = saturation;
                message = "Saturation set";
            }

        }
        else {
            // GET
            message = "Retrieved camera settings";
        }

        if (isSetup)
            message = "Restart to complete setup";

        std::cerr << "Setup result: " << message << std::endl;


        std::cerr << "onSettings:skipping" << std::endl;

        return true;

//BeeFishBScript::Object test =

        BeeFishBScript::Object output {
            {"settings", FeebeeCam::_setup},
            {"message", message},
            {"status", true},
            {"version", FeebeeCam::_setup->_beehiveVersion}
        };

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "application/json; charset=utf-8";
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << output;

        client->sendFinalChunk();

        if (isSetup) {
            FeebeeCam::_setup->_isSetup = true;
            shouldSave = true;
        }

        if (shouldSave) {
            FeebeeCam::_setup->save();
        }

        FeebeeCam::resetCameraWatchDogTimer();

        return true;
    }

    bool onRestart(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        cerr << "Restart command from web" << endl;
        
        BeeFishBScript::Object output;

        output = BeeFishBScript::Object {
            {"redirectURL", HOST "/beehive/"},
            {"message", "Restarting..."},
            {"status", true}
        };

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "application/json";
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << output.str();

        client->sendFinalChunk();

        FeebeeCam::commands.push(FeebeeCam::RESTART);

        return true;
    }

     bool onStatus(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "application/json";
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << status.str();

        client->sendFinalChunk();

        FeebeeCam::resetCameraWatchDogTimer();

        return true;
    }

}