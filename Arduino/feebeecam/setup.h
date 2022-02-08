#include "web-server-helpers.h"

namespace FeebeeCam {
    
    void onSetupPost(httpsserver::HTTPRequest * req, httpsserver::HTTPResponse * res) {

        using namespace BeeFishJSON;
        JSON json;
        JSONParser parser(json);

        BeeFishMisc::optional<BString> ssid;
        BeeFishMisc::optional<BString> password;
        BeeFishMisc::optional<BString> secretHash;

        parser.captureValue("ssid", ssid);
        parser.captureValue("password", password);
        parser.captureValue("secretHash", secretHash);

        if (!parseRequest(parser, req)) {
            res->setStatusCode(500);
            res->setStatusText("Error");
            return;
        }
        
        setDefaultHeaders(res);
        
        BeeFishJSONOutput::Object object;

        if (parser.result() == true && ssid.hasValue()) {
            std::string _ssid = ssid.value();
            std::string _password;
            if (password.hasValue())
                _password = password.value().str();

    /*
            bool updated = 
                feebeeCamConfig->update(ssid, password, secretHash);
    */
            bool updated = false;
            object["status"] = false;
            object["message"] = "Error saving feebeecam config.";
            res->setStatusCode(200);
    /*
            if (updated) {
                feebeeCamConfig->load();
                if (feebeeCamConfig->setup) {
                    INFO(TAG, "Updated FeebeeCam config");
                    object["status"] = true;
                    object["message"] = "Updated FeebeeCam config. You must restart device to continue.";
                    object["redirectURL"] = HOST "/beehive/";  
                }
                else {
                    INFO(TAG, "Error updating FeebeeCam config");
                    object["status"] = false;
                    object["message"] = "Error saving feebeecam config.";
                }
            }
            else {
                INFO(TAG, "Error updating WiFi config");
                object["status"] = false;
                object["message"] = "Error updating WiFi config";
            }
            */
        }
        
        res->printStd(object.str());
        

    }
}
