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
            "/head.js",
            "/body.js",
            "/style.css",
            "/client/console/console.js",
            "/client/style.css",
            "/client/logon/",
            "/client/logon/index.html",
            "/client/logon/style.css",
            "/client/logon/index.html",
            "/client/logon/authentication.js"
         };
         
   public:
      HTTPSAuthentication(
         Session* session,
         Headers& responseHeaders
      ) :
         App(session, responseHeaders)
      {
  
         if (authenticated())
            cerr << "HTTPS AUTH authenticated" << endl;
         else
            cerr << "HTTPS AUTH unauthenticated" << endl;
            
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
               
               if ( object.contains("secret") )
               {
                  _secret = object["secret"]->value();
               }
         
               _status = "200";
               
               if ( method == "getStatus" )
               {
               }
               else if ( method == "logon" )
               {
                  logon();
                  
                  if (authenticated())
                  {
                     _headers["set-cookie"] =
                     BString("sessionId=") +
                     _sessionId +
                  ";SameSite=None;Secure;HttpOnly;max-age=3600";
            
                  }
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
         
         responseHeaders["connection"] =
            "keep-alive";
         responseHeaders["access-control-allow-origin"] =
            origin;
            
         responseHeaders["access-control-allow-credentials"] =
            "true";
         
         if (!authenticated())
         {
            responseHeaders["set-cookie"] =
                  "sessionId=;SameSite=None;Secure;HttpOnly;max-age=0";
            
         }
         

         responseHeaders["cache-control"] =
            "no-store";
         
         if (_status == "200")
         {

            responseHeaders["content-type"] =
               "application/json; charset=UTF-8";
       
            stringstream contentStream;
            
            contentStream 
               << "{" << endl;
            Authentication
               ::write(contentStream);
             
            //contentStream
           //    << ",\"referrer\":\"";
            
           // headers["referer"]
            //   .writeEscaped(contentStream);
            contentStream
             //  << "\""
               << endl
               << "}" << "\r\n";
               
            _serveFile = false;
            _content = contentStream.str();
            
         }
         else if ( !authenticated() &&
                   !isPrivileged(
                      request.path()
                    ) )
         {
            cerr << "HTTPS AUTH Redirect " <<
                     request.path() << endl;
                     
            redirect(
               "/client/logon/",
               false
            );
            
            return;
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