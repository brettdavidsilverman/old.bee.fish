#include <file-server.h>
#include "setup.h"
#include "commands.h"
#include "wifi.h"

namespace FeebeeCam {

    Setup* _setup = nullptr;

    bool initializeSetup() {
        
        std::cerr << "Initializing setup object" << std::endl;

        FeebeeCam::_setup = new FeebeeCam::Setup();

        if (!FeebeeCam::_setup->initialize()) {
            std::cerr << "Failed to initialize setup" << std::endl;
            return false;
        }

        return true;
    }

    bool onSettings(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishBScript::Object output;
        sensor_t *sensor = esp_camera_sensor_get();

        BString message;
        bool isSetup = false;
        bool shouldSave = false;

        if (client->_webRequest.method() == "POST") {
            shouldSave = true;
            output["status"] = BeeFishBScript::Null();
            output["message"] = "Invalid setting";
            
            BeeFishBScript::Variable& request = client->_parser.value();
            BeeFishBScript::ObjectPointer object = 
                (BeeFishBScript::ObjectPointer)request;
                
            for (auto it = object->cbegin(); it != object->cend(); ++it) {

                BString key = *it;
                BeeFishBScript::Variable& value = (*object)[key];

                std::stringstream stream;
                stream << value;
                BString stringValue = stream.str();

                Serial.print("On Setting: ");
                Serial.println(key.c_str());

                const BString& setting = key;
                    
                if (setting == "frameSize") {
                    int frameSize = (int)value;
                    sensor->set_framesize(sensor, (framesize_t)frameSize);
                    FeebeeCam::_setup->_frameSize = frameSize;
                    message = "Frame size set to " + stringValue;
                }
                else if (setting == "gainCeiling") {
                    int gainCeiling = (int)value;
                    sensor->set_gainceiling(sensor, (gainceiling_t)gainCeiling);
                    FeebeeCam::_setup->_gainCeiling = gainCeiling;
                    message = "Gain ceiling set to " + stringValue;
                }
                else if (setting == "quality") {
                    int quality = (int)value;
                    sensor->set_quality(sensor, quality);
                    FeebeeCam::_setup->_quality = quality;
                    message = "Quality set to " + stringValue;
                }
                else if (setting == "brightness") {
                    int brightness = (int)value;
                    sensor->set_brightness(sensor, brightness);
                    FeebeeCam::_setup->_brightness = brightness;
                    message = "Brightness  set to " + stringValue;
                }
                else if (setting == "contrast") {
                    int contrast = (int)value;
                    sensor->set_contrast(sensor, contrast);
                    FeebeeCam::_setup->_contrast = contrast;
                    message = "Contrast  set to " + stringValue;
                }
                else if (setting == "saturation") {
                    int saturation = (int)value;
                    sensor->set_saturation(sensor, saturation);
                    FeebeeCam::_setup->_saturation = saturation;
                    message = "Saturation  set to " + stringValue;
                }
                else {
                    message = "FeebeeCam setup";
                    isSetup = true;
                    if (setting == "label") {
                        FeebeeCam::_setup->_label = value;
                    }
                    else if (setting == "ssid") {
                        FeebeeCam::_setup->_ssid = value;
                    }
                    else if (setting == "password") {
                        FeebeeCam::_setup->_password = value;
                    }
                    else if (setting == "secretHash") {
                        FeebeeCam::_setup->_secretHash = value;
                    }
                    else {
                        message = "Invalid values";
                        isSetup = false;
                    }
                }

            }


        }
        else {
            // GET
            message = "Retrieved camera settings";
        }


        output = BeeFishBScript::Object {
            {"settings", FeebeeCam::_setup->settings()},
            {"message", message},
            {"redirectURL", HOST "/beehive/"},
            {"status", true}
        };

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "text/javascript";
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << output;


        client->sendFinalChunk();

        Serial.println(message.c_str());

        bool restart = false;

        if (isSetup) {
            
            if (FeebeeCam::connectToUserSSID()) {

                // Reset the time flag
                //FeebeeCam::_setup->_isRTCSetup = false;

                // We are now officially setup
                FeebeeCam::_setup->_isSetup = true;

            }

            restart = true;

        }

        if (shouldSave)
            FeebeeCam::_setup->save();

        if (restart)
            FeebeeCam::commands.push(FeebeeCam::RESTART);

        return true;
    }

    bool onRestart(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishBScript::Object output;

        output = BeeFishBScript::Object {
            {"status", true}
        };

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "text/javascript";
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << output;

        client->sendFinalChunk();

        FeebeeCam::commands.push(FeebeeCam::RESTART);

        return true;
    }

 
}