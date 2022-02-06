#ifndef FEEBEECAM__WEB_REQUEST
#define FEEBEECAM__WEB_REQUEST

#include <bee-fish.h>
#include <config.h>
#include "web-request-base.h"
#include "json-web-request.h"

#define TAG "BeeFishWebRequest"

namespace FeebeeCam {

    class BeeFishWebRequest : public JSONWebRequest {
    protected:
        BeeFishMisc::optional<BString> _response;
        inline static BString _host = HOST;
        inline static bool _authenticated = false;

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
            static const BString DEFAULT_SECRET = "4db14a0e15e8a6a1bf1eda4dcb5e41c4db7ec311575722b88ac8b3fc0019e2f57ba2518a042f8f6292955f6187f675cee3e94564903faa069194720ff374ca55";
        public:
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

        static bool logon(BString secret = DEFAULT_SECRET) {

            Logon logon(secret);

            logon.send();

            cout << "Status Code: "
                 << logon.statusCode()
                 << endl;

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