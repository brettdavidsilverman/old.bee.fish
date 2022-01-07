#ifndef FEEBEECAM__JSON_WEB_REQUEST
#define FEEBEECAM__JSON_WEB_REQUEST
#include <bee-fish.h>
#include "web-request-base.h"

namespace FeebeeCam {

    class JSONWebRequest : public WebRequest {
    protected:
        BeeFishJSON::JSON _json;
        BeeFishJSON::JSONParser _parser;
    public:

        JSONWebRequest(
            BString host,
            BString path,
            BString query,
            BeeFishMisc::optional<BString> body = BeeFishMisc::nullopt
        ) :
            WebRequest(host, path, query, body),
            _parser(_json)
        {
        }

        virtual void send() {
            Serial.println("JSON Web Request Sending https request");
            WebRequest::send();
        }

        bool expectJSON() {
            return hasBody();
        }

        virtual void ondata(const char* data, size_t length) {
            if (expectJSON() && _parser.result() == BeeFishMisc::nullopt) {
                std::string str(data, length);
                _parser.read(str);
            }
            else
                WebRequest::ondata(data, length);
        }

        JSONParser& parser() {
            return _parser;
        }

        JSON& json() {
            return _json;
        }
            
    };

}

#endif
