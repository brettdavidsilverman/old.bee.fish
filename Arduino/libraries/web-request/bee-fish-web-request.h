#ifndef FEEBEECAM__WEB_REQUEST
#define FEEBEECAM__WEB_REQUEST

#include <bee-fish.h>
#include "web-request-base.h"
#include "json-web-request.h"

#define TAG "BeeFishWebRequest"

namespace FeebeeCam {

    class BeeFishWebRequest : public JSONWebRequest {
    protected:
        BeeFishMisc::optional<BString> _response;
        static BString _host;
        static bool _authenticated;

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
            
            if (_authenticated) {
                WebRequest::send();
            }

            if (!_authenticated || statusCode() == 401) {
                Serial.println("Unauthorized...logging in");
                // Unauthorized, try logging in and resend
                if (logon()) {
                    Serial.println("Logged in. Resending request");
                    WebRequest::send();
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
            static const BString PUBLIC_SECRET;
            Logon(BString secret) : JSONWebRequest(BeeFishWebRequest::_host, "/", "") {

                BeeFishJSONOutput::Object object = {
                    {"method", "logon"},
                    {"secret", secret}
                };

                _body = object.bstr();
 
            }

            virtual void initialize () override {
                JSONWebRequest::initialize();
                parser().captureValue("authenticated", _authenticated);
            }

            bool authenticated() {
                if  (!_authenticated.hasValue())
                    return false;
                return (_authenticated.value() == "true");
            }


        };

        static bool logon(BString secret = Logon::PUBLIC_SECRET) {

            Logon logon(secret);

            logon.send();

            BeeFishWebRequest::_authenticated = logon.authenticated();
            
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

    class BeeFishStorage : public BeeFishWebRequest {
    private:
        
        static BeeFishJSONOutput::Object& getBody(BString key, BeeFishJSONOutput::Object& value) {
            static BeeFishJSONOutput::Object object;
            
            object["method"] = "setItem";
            object["key"] = key;
            object["value"] = value.bstr();

            return object;
        }

    public:
        BeeFishStorage(
            BString path,
            BString key,
            BeeFishJSONOutput::Object& value
        ) :
            BeeFishWebRequest(
                path,
                "",
                getBody(key, value)
            )
        {

        }
        
    };
}


#endif