#ifndef BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H
#define BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H

#include "app.h"
#include "session.h"
#include "../json/json-parser.h"

namespace BeeFishHTTPS {

   using namespace BeeFishDatabase;
   using namespace BeeFishPowerEncoding;
   using namespace BeeFishHTTPS;
   using namespace BeeFishJSON;
   using namespace BeeFishWeb;
   
   class AuthenticationApp :
      public App
   {
   protected:
      inline static vector<BString>
         _privileged{
            "/",
            "/favicon.ico",
            "/feebee.jpg",
            "/feebee-small.jpg",
            "/head.js",
            "/body.js",
            "/style.css",
            "/client/console/console.js",
            "/client/style.css",
            "/client/logon/",
            "/client/logon/index.html",
            "/client/logon/logoff.html",
            "/client/logon/style.css",
            "/client/logon/sha512.js",
            "/client/logon/hash-secret.js",
            "/client/logon/authentication.js"
         };

   public:
      AuthenticationApp(
         Session* session,
         ResponseHeaders& responseHeaders
      ) : App(session, responseHeaders)
      {
      
      }

      virtual void handleResponse() {


         WebRequest* request = _session->request();

         BeeFishMisc::optional<BString> method;
         BeeFishMisc::optional<BString> secret;

         if (_session->request()->json().matched()) {

            request = new WebRequest();
            JSONParser parser(*request);
            parser.captureValue("method", method);
            parser.captureValue("secret", secret);
            
            if (!parseWebRequest(parser))
            {
               delete request;
               throw std::runtime_error("Jnvald input to https-authentication.h");
            }
            delete request;
         }

         if ( method.hasValue() )
         {
            _status = "200";
            
            if ( method == BString("getStatus") )
            {
               authenticate();
            }
            else if ( method == BString("logon") && secret.hasValue() )
            {
               logon(
                  secret.value()
               );
            }
            else if ( method == BString("logoff") )
            {
               logoff();
            }
            else
            {
               _status = "";
            }
         }
      /*
         string origin;
   
         const WebRequest::Headers&
            requestHeaders =
               request->headers();
         if (requestHeaders.contains("origin"))
            origin = (const char*)requestHeaders["origin"];
         else if (requestHeaders.contains("host"))
            origin = (const char*)requestHeaders["host"];
         else
            origin = HOST_NAME;
      */   
         _responseHeaders.replace(
            "connection",
            "keep-alive"
         );
         
         _responseHeaders.replace(
            "access-control-allow-origin",
            "*"
         );
            
         _responseHeaders.replace(
            "access-control-allow-credentials",
            "true"
         );

         if (authenticated())
         {
            
            _responseHeaders.emplace(
               "set-cookie",
               BString("sessionId=") +
               _sessionId +
               ";path=/;SameSite=None;Secure;HttpOnly;max-age=3600"
            );
         }
         else
         {
            _responseHeaders.emplace(
               "set-cookie",
               "sessionId=;path=/;SameSite=None;Secure;HttpOnly;max-age=0"
            );
         }


         _responseHeaders.replace(
            "cache-control",
            "no-store"
         );
         
         if ( !authenticated() &&
               !isPrivileged(
                  request->path()
               ) )
         {
            _serveFile = true;
            _filePath = getFilePath("/client/logon/index.html");
            _status = "401";
            
            /*
            redirect(
               "/client/logon/",
               false,
               request.path()
            );
            */
            return;
         }

         if (_status == "200")
         {

            _responseHeaders.replace(
               "content-type",
               "application/json; charset=UTF-8"
            );
            
            BeeFishJSONOutput::Object output;

            Authentication
               ::write(output);
               
            _serveFile = false;
            _content = output.str();
            
         }
         
         
      }
      
      bool isPrivileged(const BString& path)
      {
         return
            ( std::find(
                _privileged.begin(),
                _privileged.end(),
                path )
             != _privileged.end() );
      }
      
      virtual BString name()
      {
         return "HTTPS Authentication";
      }
   };
   
   

};

#endif