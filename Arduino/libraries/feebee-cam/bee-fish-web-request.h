#ifndef FEEBEECAM__WEB_REQUEST
#define FEEBEECAM__WEB_REQUEST

#include <bee-fish.h>
#include "web-request-base.h"
#include "setup.h"

namespace FeebeeCam {

    class BeeFishWebRequest : public WebRequest {
    protected:
        static const BString _host;

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

        virtual bool authenticate() {

            if (!openConnection())
                return false;

            if (!BeeFishWebRequest::Logon::authenticated()) {

                // Unauthorized, try logging in and resend
                BString secret;
                if (_setup->_secretHash.length())
                    secret = _setup->_secretHash;
                else if (strlen(BeeFishWebRequest::Logon::_lastSecret) > 0)
                    secret = BeeFishWebRequest::Logon::_lastSecret;
                else
                    secret = BeeFishWebRequest::Logon::PUBLIC_SECRET;

                if (BeeFishWebRequest::logon(secret)) {
                    cerr << "Logged in." << endl;
                }
                
            }

            return BeeFishWebRequest::Logon::authenticated();


        }

        virtual bool send() {

            if (!authenticate())
               return false;

            return WebRequest::send();

        }



        class Logon : public WebRequest {
        public:
            static const BString PUBLIC_SECRET;
            static RTC_DATA_ATTR bool _authenticated;
            static RTC_DATA_ATTR char _lastSecret[512];
            static RTC_DATA_ATTR long _timeout;

            Logon(BString secret) :
                WebRequest(BeeFishWebRequest::_host, "/logon", "", true)
            {
                _method = "POST";

                _body = BeeFishBScript::Object {
                    {"method", "logon"},
                    {"secret", secret}
                };

                memcpy(Logon::_lastSecret, secret.c_str(), secret.length());

                _hasBody = true;
            }

            static bool authenticated() {
                return 
                    BeeFishWebRequest::Logon::_authenticated && 
                    BeeFishWebRequest::Logon::_timeout > millis();
            }

            virtual bool send() {

                _authenticated = false;
                
                if (!WebRequest::send()) {
                    return false;
                }

                if (_webResponse && _webResponse->matched()) {
                    BeeFishBScript::ObjectPointer object = _parser->json();

                    unsigned long expiryMillis = millis() + 60 * 1000;

                    if (object->contains("timeout"))  {
                        expiryMillis = millis() + (BeeFishBScript::Number)(*object)["timeout"] * 1000;
                    }

                    BeeFishWebRequest::Logon::_timeout = expiryMillis;

                    _authenticated = (
                        object->contains("authenticated") && 
                        (*object)["authenticated"] == true
                    );

                }

                return _authenticated;

            }

        };

        static bool logon(BString secret = Logon::PUBLIC_SECRET) {

            if ( secret == Logon::_lastSecret &&
                 BeeFishWebRequest::Logon::authenticated())
            {
                cerr << "Already authenticated" << endl;
                return true;
            }

            cerr << "Logging on to " << _host << endl;

            memset(Logon::_lastSecret, 0, sizeof(Logon::_lastSecret));

            Logon logon(secret);

            if (!logon.send()) {
                cerr << "Error with logon" << endl;
            }
            
            bool authenticated;

            if (authenticated = Logon::authenticated()) {
                cerr << "Authenticated" << endl;
            }
            else {
                cerr 
                    << "Not authenticated: "
                    << logon.statusCode() << endl;
            }

            return authenticated;
        }

        static bool logoff() {
            WebRequest::setCookie("");
            BeeFishWebRequest::Logon::_authenticated = false;
            memset(Logon::_lastSecret, 0, sizeof(Logon::_lastSecret));
            return true;
        }

 
    };

}


#endif