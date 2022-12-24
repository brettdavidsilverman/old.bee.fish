#include <file-server.h>
#include "setup.h"
#include "commands.h"
#include "wifi.h"
#include "camera.h"

namespace FeebeeCam {

    Setup* _setup = nullptr;

    using namespace fs;

    bool initializeSetup() {
        
        std::cerr << "Initializing setup object" << std::endl;

        FeebeeCam::_setup = new FeebeeCam::Setup();

        return FeebeeCam::_setup->inititalize();
    }

    bool onSetupBeehive(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        

        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        sensor_t *sensor = esp_camera_sensor_get();

        BString message;
        bool isSetup = false;

        if (client->_webRequest.method() == "POST") {


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
                int frameSize = (int)(BeeFishBScript::Number)(*input)["frameSize"];
                sensor->set_framesize(sensor, (framesize_t)frameSize);
                FeebeeCam::_setup->_frameSize = frameSize;
                message = "Frame size set";
            }
            
            if (input->contains("gainCeiling")) {
                int gainCeiling = (int)(BeeFishBScript::Number)(*input)["gainCeiling"];
                sensor->set_gainceiling(sensor, (gainceiling_t)gainCeiling);
                FeebeeCam::_setup->_gainCeiling = gainCeiling;
                message = "Gain ceiling set";
            }
            
            if (input->contains("quality")) {
                int quality = (int)(BeeFishBScript::Number)(*input)["quality"];
                sensor->set_quality(sensor, quality);
                FeebeeCam::_setup->_quality = quality;
                message = "Quality set";
            }
            
            if (input->contains("brightness")) {
                int brightness = (int)(BeeFishBScript::Number)(*input)["brightness"];
                sensor->set_brightness(sensor, brightness);
                FeebeeCam::_setup->_brightness = brightness;
                message = "Brightness set";
            }
            
            if (input->contains("contrast")) {
                int contrast = (int)(BeeFishBScript::Number)(*input)["contrast"];
                sensor->set_contrast(sensor, contrast);
                FeebeeCam::_setup->_contrast = contrast;
                message = "Contrast set";
            }
            
            if (input->contains("saturation")) {
                int saturation = (int)(BeeFishBScript::Number)(*input)["saturation"];
                sensor->set_saturation(sensor, saturation);
                FeebeeCam::_setup->_saturation = saturation;
                message = "Saturation set";
            }

        }
        else {
            // GET
            message = "Retrieved camera setup";
        }

        if (isSetup)
            message = "Restart to complete setup";

        std::cerr << "Setup result: " << message << std::endl;

        FeebeeCam::_setup->assign(false);
        
        BeeFishBScript::Object output {
            {"setup", (*FeebeeCam::_setup)},
            {"message", message},
            {"status", true},
            {"version", FeebeeCam::_setup->_beehiveVersion}
        };

        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "application/json; charset=utf-8";
        client->_contentLength = output.contentLength();

        client->sendHeaders();

        //BeeFishBString::BStream& stream = client->getChunkedOutputStream();
        BeeFishBString::BStream& stream = client->getOutputStream();

        stream << output;

        //client->sendFinalChunk();
        stream.flush();

        if (isSetup) {
            FeebeeCam::_setup->_isSetup = true;
            FeebeeCam::_setup->_isRTCSetup = false;
            FeebeeCam::_setup->save();
        }

        FeebeeCam::resetCameraWatchDogTimer();

        return true;
    }

    bool onRedirect(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        using namespace BeeFishBString;

        if (path == "/setup")
            return FeebeeCam::onFileServer(path, client);

        cerr << "REDIRECT FROM PATH: " << path << endl;

        // This redirect is necessary for captive portal
        BString redirect = FeebeeCam::getURL() + "setup";

        std::cerr << "Redireccting all trafic to " << redirect << std::endl;

        BStream stream = client->getOutputStream();

        stream << "HTTP/1.1 " << 302 << " " << "Redirect" << "\r\n"
                "server: FeebeeCam server" <<  "\r\n" <<
                "location: " << redirect << "\r\n" <<
                "\r\n";

        stream.flush();

        return true;
    }

    bool onGenerate204(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        using namespace BeeFishBString;

        std::cerr << "Generating 204 "<< std::endl;

        BStream stream = client->getOutputStream();

        stream << "HTTP/1.1 " << 204 << " " << "No Content response" << "\r\n"
                "server: FeebeeCam server" <<  "\r\n" <<
                "\r\n";

        stream.flush();

        return true;
    }
/*
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
        client->_chunkedEncoding = true;
        
        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getChunkedOutputStream();

        stream << output.str();

        client->sendFinalChunk();

        FeebeeCam::commands.push(FeebeeCam::RESTART);

        return true;
    }
*/

}