#ifndef FEEBEECAM__WEB_REQUEST
#define FEEBEECAM__WEB_REQUEST

#include <bee-fish.h>
#include "web-request-base.h"
#include "setup.h"

namespace FeebeeCam {

    class BeeFishWebRequest : public WebRequest {
    protected:
        static BString _host;
        static bool _authenticated;

    public:
        BeeFishWebRequest(
            BString path, 
            BString query = "",
            bool hasBody = false
        ) :
            WebRequest(_host, path, query, hasBody)
        {
        }

        virtual ~BeeFishWebRequest() {
        }

        virtual void setPath(const BString& path) {
            _path = path;
        }

        virtual bool send() {
            
            bool sent = false;

            _authenticated = false;

            sent = WebRequest::send();

            if (WebRequest::statusCode() == 401) {
                _authenticated = false;
                
                Serial.println("Unauthorized...logging in");

                // Unauthorized, try logging in and resend
                BString secret;
                if (BeeFishWebRequest::Logon::_lastSecret.length())
                    secret = BeeFishWebRequest::Logon::_lastSecret;
                else if (setup._secretHash.length())
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



        class Logon : public WebRequest {
        public:
            static const BString PUBLIC_SECRET;
            static BString _lastSecret;

            Logon(BString secret) :
                 WebRequest(BeeFishWebRequest::_host, "/", "", true)
            {
                _method = "POST";
                _body = {
                    {"method", "logon"},
                    {"secret", secret}
                };

                _hasBody = true;
            }

            bool authenticated() {
                return (_webResponse && _webResponse->authenticated());
            }

        };

        static bool logon(BString secret = Logon::PUBLIC_SECRET) {

            if (secret == Logon::_lastSecret && BeeFishWebRequest::_authenticated) {
                Serial.println("Already authenticated");
                return true;
            }

            Logon::_lastSecret = "";

            Serial.print("Logging on to ");
            Serial.println(_host.c_str());

            Logon logon(secret);

            if (logon.send()) {
                BeeFishWebRequest::_authenticated = logon.authenticated();
                if (BeeFishWebRequest::_authenticated)
                    Logon::_lastSecret = secret;
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
            BeeFishWebRequest::Logon::_lastSecret.clear();
            return true;
        }

    };

}


#endif