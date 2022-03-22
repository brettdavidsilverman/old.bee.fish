#ifndef FEEBEECAM__WEB_REQUEST
#define FEEBEECAM__WEB_REQUEST

#include <bee-fish.h>
#include "web-request-base.h"
#include "setup.h"

namespace FeebeeCam {

    class BeeFishWebRequest : public WebRequest {
    protected:
        BeeFishMisc::optional<BString> _response;
        static BString _host;
        static bool _authenticated;

    public:
        BeeFishWebRequest(
            BString path, 
            BString query = ""
        ) :
            WebRequest(_host, path, query)
        {

        }

        BeeFishWebRequest(
            BString path, 
            BString query,
            BeeFishJSONOutput::Object& body
        ) :
            WebRequest(_host, path, query, body.bstr())
        {

        }

        virtual bool send() {
            
            bool sent = false;
            
            if (_authenticated) {
                sent = WebRequest::send();
                if (WebRequest::statusCode() == 401) {
                    _authenticated = false;
                }
            }

            if (!sent && !_authenticated) {
                Serial.println("Unauthorized...logging in");
                // Unauthorized, try logging in and resend
                Setup setup;
                BString secret;
                if (setup._secretHash.length())
                    secret = setup._secretHash;
                else
                    secret = BeeFishWebRequest::Logon::PUBLIC_SECRET;

                if (BeeFishWebRequest::logon(secret)) {
                    _authenticated = true;
                    Serial.println("Logged in. Resending request");
                    sent = WebRequest::send();
                }
            }

            return sent;
        }


        const BString& response() {
            return _response.value();
        }


        class Logon : public WebRequest {
        public:
            static const BString PUBLIC_SECRET;
            Logon(BString secret) : WebRequest(BeeFishWebRequest::_host, "/", "") {

                BeeFishJSONOutput::Object object = {
                    {"method", "logon"},
                    {"secret", secret}
                };

                _body = object.bstr();
                _method = "POST";
            }

            bool authenticated() {
                return _webResponse->authenticated();
            }

        };

        static bool logon(BString secret = Logon::PUBLIC_SECRET) {

            Serial.print("Logging on to ");
            Serial.println(_host.c_str());

            Logon logon(secret);

            if (logon.send()) {
                BeeFishWebRequest::_authenticated = logon.authenticated();
            }
            else {
                Serial.println("Error with logon");
                BeeFishWebRequest::_authenticated = false;
            }
            
            if (logon.authenticated()) {
                Serial.print("Authenticated");
            }
            else {
                Serial.print("Not authenticated ");
                Serial.print(logon.statusCode());
            }

            Serial.println();

            return logon.authenticated();
        }

        static bool logoff() {
            WebRequest::cookie() = BeeFishMisc::nullopt;
            BeeFishWebRequest::_authenticated = false;
            return true;
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