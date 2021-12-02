#ifndef BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H
#define BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H

#include "app.h"
#include "session.h"

using namespace BeeFishDatabase;
using namespace BeeFishPowerEncoding;
using namespace BeeFishHTTPS;

namespace BeeFishHTTPS {

   class HTTPSAuthentication :
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
            "/client/logon/sha256.js",
            "/client/logon/hash-secret.js",
            "/client/logon/authentication.js"
         };

   public:
      HTTPSAuthentication(
         Session* session,
         ResponseHeaders& responseHeaders
      ) : App(session, responseHeaders)
      {
      
      }

      virtual void handleResponse() {

         cerr << "HTTPSAuthentication::HTTPSAuthentication" << endl;


         BeeFishMisc::optional<BString> method;
         BeeFishMisc::optional<BString> secret;

         cerr << "HTTPSAuthentication::Setting captures" << endl;

         Request request;

         request.json().captureObjectField("method", method);
         request.json().captureObjectField("secret", secret);
         
         cerr << "HTTPSAuthentication::Parse request" << endl;

         parseRequest(request);

         cerr << "HTTPSAuthentication::Checking has json" << endl;

         if ( request.json().result() == true )
         {
            cerr << "HTTPSAuthentication::has json" << endl;
            _status = "200";
            
            if ( method == BString("getStatus") )
            {
            }
            else if ( method == BString("logon") && secret.hasValue())
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

         cerr << "HTTPSAuthentication::Setting origin" << endl;
         
         string origin;
   
         const Request::Headers&
            requestHeaders =
               request.headers();
      
         if (requestHeaders.contains("origin"))
            origin = requestHeaders["origin"];
         else if (requestHeaders.contains("host"))
            origin = requestHeaders["host"];
         else
            origin = HOST_NAME;
         
         _responseHeaders.replace(
            "connection",
            "keep-alive"
         );
         
         _responseHeaders.replace(
            "access-control-allow-origin",
            origin
         );
            
         _responseHeaders.replace(
            "access-control-allow-credentials",
            "true"
         );

         cerr << "HTTPSAuthentication::Checking authenticated() " << endl;

         if (authenticated())
         {
            
            cerr << "HTTPSAuthentication:authenticated " << endl;
            _responseHeaders.emplace(
               "set-cookie",
               BString("sessionId=") +
               _sessionId +
               ";path=/;SameSite=None;Secure;HttpOnly;max-age=3600"
            );
         }
         else
         {
            cerr << "HTTPSAuthentication:not authenticated " << endl;
            _responseHeaders.emplace(
               "set-cookie",
               "sessionId=;path=/;SameSite=None;Secure;HttpOnly;max-age=0"
            );
         }


         _responseHeaders.replace(
            "cache-control",
            "no-store"
         );
         
         cerr << "HTTPSAuthentication::check is privileged: " << request.path() << ":" << isPrivileged(request.path()) << endl;
         
         if ( !authenticated() &&
               !isPrivileged(
                  request.path()
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
            cerr << "HTTPSAuthentication::HTTPSAuthentication return exit " << endl;
            return;
         }
         if (_status == "200")
         {

            _responseHeaders.replace(
               "content-type",
               "application/json; charset=UTF-8"
            );
       
            stringstream contentStream;
            
            contentStream 
               << "{" << endl;
               
            Authentication
               ::write(contentStream);
               
            contentStream
                << endl << "}" << "\r\n";
               
            _serveFile = false;
            _content = contentStream.str();
            
         }
         
         
         cerr << "HTTPSAuthentication::HTTPSAuthentication exit " << endl;
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