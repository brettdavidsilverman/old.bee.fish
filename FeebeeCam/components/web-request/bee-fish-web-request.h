#ifndef FEEBEECAM__WEB_REQUEST
#define FEEBEECAM__WEB_REQUEST

#include <bee-fish.h>
#include <config.h>
#include "web-request-base.h"
#include "json-web-request.h"
#include "feebee-cam-config.h"

namespace FeebeeCam {

    class BeeFishWebRequest : public JSONWebRequest {
    protected:
        BeeFishMisc::optional<BString> _status;
        inline static const BString _host = HOST;
    public:
        BeeFishWebRequest(
            BString path, 
            BString query = ""
        ) :
            JSONWebRequest(_host, path, query)
        {

        }

        BeeFishWebRequest(
            BString path, 
            BString query,
            BeeFishJSONOutput::Object& body
        ) :
            JSONWebRequest(_host, path, query, body.bstr())
        {

        }

        virtual void send() {
            Serial.println("Bee Fish Web Request Sending https request");
            JSONWebRequest::send();
            if (statusCode() == 401) {
                Serial.println("Unauthorized...logging in");
                // Unauthorized, try logging in and resend
                if (logon()) {
                    Serial.println("Logged in. Resending request");
                    JSONWebRequest::send();
                }
            }
        }

        static bool logon() {

            BeeFishJSONOutput::Object logon;
        
            logon["method"] = "logon";
            logon["secret"] = feebeeCamConfig->getSecretHash();

            JSONWebRequest webRequest(_host, "/", "", logon.bstr());

            BeeFishMisc::optional<BString> authenticated;

            webRequest.parser().captureValue("authenticated", authenticated);

            cout << "Logging in...";
      
            webRequest.send();

            cout << "Status Code: "
                 << webRequest.statusCode()
                 << " authenticated: " << authenticated
                 << endl;

            if (webRequest.statusCode() == 200) {

                return authenticated == BString("true");
            }

            return false;
        }

    };
}


#endif