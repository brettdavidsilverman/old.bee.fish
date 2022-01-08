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
        BeeFishMisc::optional<BString> _response;
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

        virtual void initialize() {
            JSONWebRequest::initialize();
            parser().captureValue("response", _response);
        }

        const BString& response() {
            return _response.value();
        }


        class Logon : public JSONWebRequest {
        protected:
            BeeFishMisc::optional<BString> _authenticated;
        public:
            Logon() : JSONWebRequest(BeeFishWebRequest::_host, "/", "") {

                BeeFishJSONOutput::Object object = {
                    {"method", "logon"},
                    {"secret", feebeeCamConfig->getSecretHash()}
                };

                _body = object.bstr();
 
            }

            virtual void initialize() {
                JSONWebRequest::initialize();
                parser().captureValue("authenticated", _authenticated);
            }

            bool authenticated() {
                if  (!_authenticated.hasValue())
                    return false;
                return (_authenticated.value() == "true");
            }

        };

        static bool logon() {

            if (!feebeeCamConfig->setup) {
                cout << "Config not setup" << endl;;
                return false;
            }

            cout << "Logging in...";

            Logon logon;

            logon.send();

            cout << "Status Code: "
                 << logon.statusCode()
                 << endl;

            if (logon.authenticated()) {
                cout << "Authenticated";
            }
            else {
                cout << "Not authenticated";
            }

            cout << endl;

            return logon.authenticated();
        }

    };
}


#endif