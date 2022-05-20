#include <file-server.h>
#include "setup.h"

namespace FeebeeCam {

    Setup _setup;

    bool onSetupSettings(BeeFishWeb::WebRequest& request, WiFiClient& client) {

        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;
        BeeFishBScript::Object output;

        if (request.method() == "POST") {

            BeeFishJSON::Object::OnKeyValue onsetting =
                
                [](const BString& key, JSON& json) {

                    const BString& value = json.value();

                    if (key == "label")
                        _setup._label = value;
                    else if (key == "ssid")
                        _setup._ssid = value;
                    else if (key == "password")
                        _setup._password = value;
                    else if (key == "secretHash")
                        _setup._secretHash = value;
                };

            BeeFishJSON::Object json;

            BeeFishJSON::JSONParser parser(json);
            json.setOnKeyValue(onsetting);

            if (WiFiWebServer::parseRequest(parser, client)) {
                _setup.save();
                output["status"] = true;
                output["message"] = "Setup complete";
                output["redirectURL"] = HOST "/beehive/";
            } else {
                output["status"] = false;
                output["message"] = "Setup failed";
            }
        }

        output["label"] = _setup._label;
        output["ssid"] = _setup._ssid;

        WiFiWebServer::sendResponse(client, output);

        return true;
    }

 
}