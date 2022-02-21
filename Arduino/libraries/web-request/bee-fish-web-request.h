#ifndef FEEBEECAM__WEB_REQUEST
#define FEEBEECAM__WEB_REQUEST

#include <bee-fish.h>
#include "web-request-base.h"

#define TAG "BeeFishWebRequest"

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
            
            if (_authenticated) {
                return WebRequest::send();
            }

            if (!_authenticated || statusCode() == 401) {
                Serial.println("Unauthorized...logging in");
                // Unauthorized, try logging in and resend
                if (BeeFishWebRequest::logon()) {
                    Serial.println("Logged in. Resending request");
                    return WebRequest::send();
                }
            }

            return false;
        }


        const BString& response() {
            return _response.value();
        }


        class Logon : public WebRequest {
        protected:
            bool _authenticated = false;
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

            virtual void initialize () override {
                WebRequest::initialize();
                BeeFishParser::Match* body = _webResponse->body();
                BeeFishWeb::JSONWebResponseBody* jsonBody = 
                    (BeeFishWeb::JSONWebResponseBody*)body;

                jsonBody->setOnKeyValue(
                    [this](const BString& key, BeeFishJSON::JSON& value) {
                        if (key == "authenticated") {
                            this->_authenticated = (value.value() == "true");
                        }
                    }
                );
            }

            bool authenticated() {
                return _authenticated;
            }


        };

        static bool logon(BString secret = Logon::PUBLIC_SECRET) {

            Serial.print("Logging on to ");
            Serial.println(_host.c_str());

            Logon logon(secret);

            if (logon.send()) {
                Serial.println("Logon attempt sent and parsed");
                BeeFishWebRequest::_authenticated = logon.authenticated();
            }
            else {
                Serial.println("Error with logon");
            }
            
            if (logon.authenticated()) {
                cout << "Authenticated";
            }
            else {
                cout << "Not authenticated " << logon.statusCode();
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