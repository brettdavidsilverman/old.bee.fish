#include <Arduino.h>
#include "camera.h"
#include "light.h"

namespace FeebeeCam {

    bool onCommandPost(BeeFishWeb::WebRequest& request, WiFiClient& client) {
        
        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;

        BeeFishJSONOutput::Object object;
        object["status"] = BeeFishJSONOutput::Null();
        object["message"] = "Invalid command";
        
        // Command
        BString command;
        JSONParser::OnValue oncommand = 
            [&object, &command, &client](const BString& key, JSON& json) {
                command = json.value();
                camera_fb_t* fb = nullptr;
                if (command == "stop") {
                    FeebeeCam::stop = true;
                    object["status"] = true;
                    object["message"] = "Camera stopped";
                }
                else if (command == "save") {
                    esp_camera_save_to_nvs("user");
                    object["status"] = true;
                    object["message"] = "User settings saved";
                }

                WiFiWebServer::sendResponse(client, object);

                if (fb)
                    esp_camera_fb_return(fb);

            };
        
        BeeFishJSON::JSON json;

        BeeFishJSON::JSONParser parser(json);

        parser.invokeValue("command", oncommand);

        WiFiWebServer::parseRequest(parser, client);
        
        Serial.print("Sent Camera command ");
        Serial.println(command.c_str());

        return true;

    }
}