#include <file-server.h>
#include "setup.h"

namespace FeebeeCam {

    bool onSetupSettings(BeeFishWeb::WebRequest& request, WiFiClient& client) {

        using namespace BeeFishBString;
        using namespace BeeFishJSON;
        using namespace BeeFishParser;
        BeeFishJSONOutput::Object output;

        Setup setup;
        
        if (request.method() == "POST") {

            BeeFishJSON::Object::OnKeyValue onsetting =
                
                [&setup](const BString& key, JSON& json) {

                    const BString& value = json.value();

                    if (key == "label")
                        setup._label = value;
                    else if (key == "ssid")
                        setup._ssid = value;
                    else if (key == "password")
                        setup._password = value;
                    else if (key == "secretHash")
                        setup._secretHash = value;
                };

            BeeFishJSON::JSON json;

            BeeFishJSON::JSONParser parser(json);
            json.setup(&parser);
            json._object->setOnKeyValue(onsetting);

            if (WiFiWebServer::parseRequest(parser, client)) {
                setup.save();
                output["status"] = true;
                output["message"] = "Setup complete";
                output["redirectURL"] = HOST "/beehive/";
            } else {
                output["status"] = false;
                output["message"] = "Setup failed";
            }
        }

        output["label"] = setup._label;
        output["ssid"] = setup._ssid;

        WiFiWebServer::sendResponse(client, output);

        return true;
    }
 
}