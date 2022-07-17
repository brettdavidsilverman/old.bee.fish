#include <file-server.h>
#include "setup.h"
#include "commands.h"

namespace FeebeeCam {

    Setup setup;

    bool onSettings(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishBScript::Object output;
        sensor_t *sensor = esp_camera_sensor_get();

        BString message;

        if (client->_webRequest.method() == "POST") {

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
                    setup._frameSize = frameSize;
                    message = "Frame size set to " + stringValue;
                }
                else if (setting == "gainCeiling") {
                    int gainCeiling = (int)value;
                    sensor->set_gainceiling(sensor, (gainceiling_t)gainCeiling);
                    setup._gainCeiling = gainCeiling;
                    message = "Gain ceiling set to " + stringValue;
                }
                else if (setting == "quality") {
                    int quality = (int)value;
                    sensor->set_quality(sensor, quality);
                    setup._quality = quality;
                    message = "Quality set to " + stringValue;
                }
                else if (setting == "brightness") {
                    int brightness = (int)value;
                    sensor->set_brightness(sensor, brightness);
                    setup._brightness = brightness;
                    message = "Brightness  set to " + stringValue;
                }
                else if (setting == "contrast") {
                    int contrast = (int)value;
                    sensor->set_contrast(sensor, contrast);
                    setup._contrast = contrast;
                    message = "Contrast  set to " + stringValue;
                }
                else if (setting == "saturation") {
                    int saturation = (int)value;
                    sensor->set_saturation(sensor, saturation);
                    setup._saturation = saturation;
                    message = "Saturation  set to " + stringValue;
                }
                else {
                    message = "FeebeeCam setup";
                    if (setting == "label") {
                        setup._label = value;
                    }
                    else if (setting == "ssid") {
                        setup._ssid = value;
                    }
                    else if (setting == "password") {
                        setup._password = value;
                    }
                    else if (setting == "secretHash") {
                        setup._secretHash = value;
                    }
                    else {
                        message = "Invalid values";
                    }
                }

            }

            setup.save();

        }
        else {
            // GET
            message = "Retrieved camera settings";
        }


        output = BeeFishBScript::Object {
            {"settings", setup.settings()},
            {"message", message},
            {"redirectURL", HOST "/beehive/"},
            {"status", true}
        };

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "text/javascript";
        
        client->sendHeaders();

        BeeFishBString::BStream stream = client->getChunkedOutputStream();

        stream << output;

        stream.flush();

        client->sendChunk();

        Serial.println(message.c_str());

       
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

        BeeFishBString::BStream stream = client->getChunkedOutputStream();

        stream << output;

        stream.flush();

        client->sendChunk();

        FeebeeCam::commands.push(FeebeeCam::RESTART);

        return true;
    }

 
}