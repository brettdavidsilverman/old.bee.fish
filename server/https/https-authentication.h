#ifndef BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H
#define BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H

#include "app.h"
#include "session.h"

using namespace bee::fish::database;
using namespace bee::fish::power_encoding;
using namespace bee::fish::https;

namespace bee::fish::https {

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
            "/client/logon/style.css",
            "/client/logon/index.html",
            "/client/logon/sha256.js",
            "/client/logon/hash-secret.js",
            "/client/logon/authentication.js"
         };
         
   public:
      HTTPSAuthentication(
         Session* session,
         ResponseHeaders& responseHeaders
      ) :
         App(session, responseHeaders)
      {
  
         Request& request =
            *(session->request());
            
         BString method;
         
         if ( request.hasJSON() )
         {
            _Object& object =
               *(request.json()._object);

            if ( object.matched() )
                 
            {
               
               
               if ( object.contains("method") )
               {
                  method =
                     object["method"]->value();
               }
               
               bool hasSecret = object.contains("secret");
         
               _status = "200";
               
               if ( method == "getStatus" )
               {
               }
               else if ( method == "logon" && hasSecret )
               {
                  logon(
                     object["secret"]->value()
                  );
                  
               }
               else if ( method == "logoff" )
               {
                  logoff();
               }
               else
               {
                  _status = "";
               }
            }
            
            
         }
         
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
         
         responseHeaders.replace(
            "connection",
            "keep-alive"
         );
         
         responseHeaders.replace(
            "access-control-allow-origin",
            origin
         );
            
         responseHeaders.replace(
            "access-control-allow-credentials",
            "true"
         );
            
         if (authenticated())
         {
            
            responseHeaders.emplace(
               "set-cookie",
               BString("sessionId=") +
               _sessionId +
               ";path=/;SameSite=None;Secure;HttpOnly;max-age=3600"
            );
         }
         else
         {
            responseHeaders.emplace(
               "set-cookie",
               "sessionId=;path=/;SameSite=None;Secure;HttpOnly;max-age=0"
            );
         }


         responseHeaders.replace(
            "cache-control",
            "no-store"
         );
         
         cout << "Authenticated: ";
         if (authenticated())
            cout << "true";
         else
            cout << "false";

         cout << endl;

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
            return;
         }
         if (_status == "200")
         {

            responseHeaders.replace(
               "content-type",
               "application/json; charset=UTF-8"
            );
       
            wstringstream contentStream;
            
            contentStream 
               << "{" << endl;
               
            Authentication
               ::write(contentStream);
               
            contentStream
                << endl << "}" << "\r\n";
               
            _serveFile = false;
            _content = wstr2str(
               contentStream.str()
            );
            
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