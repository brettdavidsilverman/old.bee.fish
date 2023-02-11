#include <file-server.h>
#include "setup.h"
#include "commands.h"
#include "wifi.h"
#include "camera.h"
#include "status.h"

namespace FeebeeCam {

    Setup* _setup = nullptr;

    using namespace fs;

    bool initializeSetup() {
        
        std::cerr << "Initializing setup object" << std::endl;

        if (FeebeeCam::_setup)
            delete FeebeeCam::_setup;

        FeebeeCam::_setup = new FeebeeCam::Setup();

        bool success = FeebeeCam::_setup->inititalize();

        if (success)
            FeebeeCam::_setup->applyToCamera();

        return success;

    }
/*
    bool onSetup(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {

        if (path.toLower() == "/setup.json") {
            return onSetupBeehiveJSON(path, client);
        }
        else if (path.toLower() == "/downloadstatus") {
            return FeebeeCam::onDownloadStatus(path, client);
        }
        else if (path.toLower() == "/command") {
            return FeebeeCam::onCommand(path, client);
        }
        else if (!serveFile(path, client)) {

            BString redirect = 
                BString("http://") +
                WiFi.softAPIP().toString().c_str() + 
                BString("/setup");

            cerr << "Redirecting: " << redirect << endl;

            BStream stream = client->getOutputStream();
            BString content = "Redirecting to " + redirect;

            stream << 
                "HTTP/1.1 " << 302 << " " << "Redirect" << "\r\n"
                "server: FeebeeCam server" <<  "\r\n" <<
                "location: " << redirect << "\r\n" <<
                "content-length: " << content.length() << "\r\n" <<
                "content-type: " << "text/plain" << "\r\n" <<
                "\r\n" <<
                content;

            stream.flush();

            return true;
        }
        
        return FeebeeCam::onFileServer(path, client);


    }
*/
    bool onSetupBeehiveJSON(const BeeFishBString::BString& path, FeebeeCam::WebClient* client) {
        

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

            if (input->contains("hostSSID")) {
                isSetup = true;
                FeebeeCam::_setup->_hostSSID = (*input)["hostSSID"];
            }

            if (input->contains("hostPassword")) {
                isSetup = true;
                FeebeeCam::_setup->_hostPassword = (*input)["hostPassword"];
            }

            if (input->contains("feebeeCamSSID")) {
                isSetup = true;
                FeebeeCam::_setup->_feebeeCamSSID = (*input)["feebeeCamSSID"];
            }

            if (input->contains("feebeeCamPassword")) {
                isSetup = true;
                FeebeeCam::_setup->_feebeeCamPassword = (*input)["feebeeCamPassword"];
            }

            if (input->contains("timeZone")) {
                isSetup = true;
                FeebeeCam::_setup->_timeZone = (*input)["timeZone"];
            }

            if (input->contains("timeZoneLabel")) {
                isSetup = true;
                FeebeeCam::_setup->_timeZoneLabel = (*input)["timeZoneLabel"];
            }

            if (input->contains("secretHash")) {
                isSetup = true;
                FeebeeCam::_setup->_secretHash = (*input)["secretHash"];
            }

            if (input->contains("wakeupEvery")) {
                BeeFishBScript::Number wakeupEvery = (*input)["wakeupEvery"];
                FeebeeCam::_setup->_wakeupEvery = wakeupEvery;
                isSetup = true;
                message = "Wakeup every set";
            }

            if (input->contains("takePictureEvery")) {
                int takePictureEvery = (int)(BeeFishBScript::Number)(*input)["takePictureEvery"];
                FeebeeCam::_setup->_takePictureEvery = takePictureEvery;
                isSetup = true;
                message = "Take picture every set";
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

        if (isSetup) {
            FeebeeCam::_setup->_isSetup = true;
            FeebeeCam::_setup->save();
            message = "Restart to complete setup";
        }

        std::cerr << "Setup result: " << message << std::endl;

        BeeFishBScript::Object copy;

        FeebeeCam::_setup->assign(copy, false);
        
        client->_statusCode = 200;
        client->_statusText = "OK";
        client->_contentType = "application/json; charset=utf-8";
        client->_contentLength = copy.contentLength();

        client->sendHeaders();

        BeeFishBString::BStream& stream = client->getOutputStream();

        stream << copy;

        client->flush();

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