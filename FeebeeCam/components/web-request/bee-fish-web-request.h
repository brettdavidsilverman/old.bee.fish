#include <bee-fish.h>
#include "web-request-base.h"
#include "feebee-cam-config.h"

namespace BeeFish {

    class BeeFishWebRequest : public FeebeeCam::WebRequest {
    protected:
//        BeeFishJSON::JSONParser _parser;
        BeeFishMisc::optional<BString> _status;
        const BString _host = "http://laptop";
    protected:
        BeeFishWebRequest(
            BString path = "/",
            BString query = "",
            BeeFishMisc::optional<BString> body = BeeFishMisc::nullopt
        ) :
            WebRequest(_host, path, query, body) 
        {
            //_parser.capture("status", _status);
        }

    public:
        BeeFishWebRequest(
            BString path = "/", 
            BString query = ""
        ) :
            BeeFishWebRequest(path, query, BeeFishMisc::nullopt)
        {

        }

        BeeFishWebRequest(
            BString path, 
            BString query,
            BeeFishJSONOutput::Object& body
        ) :
            BeeFishWebRequest(path, query, getBodyString(body)) 
        {

        }

        static BString getBodyString(BeeFishJSONOutput::Object& body) {
            body["secretHash"] = feebeeCamConfig._secretHash;
            return body.str();
        }

        virtual void ondata(const char* data, size_t length) {
            WebRequest::ondata(data, length);
        }
    };
}

