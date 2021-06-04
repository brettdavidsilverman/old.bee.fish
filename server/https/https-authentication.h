#ifndef BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H
#define BEE_FISH_HTTPS__HTTPS_AUTHENTICATION_H
#include "authentication.h"
#include "app.h"
using namespace bee::fish::database;
using namespace bee::fish::power_encoding;
using namespace bee::fish::https;

namespace bee::fish::https {

   class HTTPSAuthentication :
      public Authentication,
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
            "/client/sha3/sha3.js",
            "/client/sha3/hash-file.js",
            "/client/style.css",
            "/client/logon/",
            "/client/logon/index.html",
            "/client/logon/style.css",
            "/client/logon/index.html",
            "/client/logon/authentication.js"
         };
         
   public:
      HTTPSAuthentication(
         Session* session
      ) :
         Authentication(session),
         App(session)
      {
  
         if (authenticated())
            cerr << "HTTPS AUTH authenticated" << endl;
         else
            cerr << "HTTPS AUTH unauthenticated" << endl;
            
         Request& request =
            *(session->request());
        
         if ( request.hasJSON() )
         {
            _Object& object =
               *(request.json()._object);

            if ( object.matched() )
                 
            {
               BString method;
               
               if ( object.contains("method") )
               {
                  method =
                     object["method"]->value();
               }
               
               if ( object.contains("name") )
               {
                  _name = object["name"]->value();
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
   
         const Request::Headers& headers =
            request.headers();
      
         if (headers.contains("origin"))
            origin = headers["origin"];
         else if (headers.contains("host"))
            origin = headers["host"];
         else
            origin = HOST_NAME;
         
         _headers["connection"] =
            "keep-alive";
      
         if (authenticated())
            _headers["set-cookie"] =
               BString("sessionId=") +
               _sessionId +
               ";SameSite=None;Secure;HttpOnly;max-age=3600";
         else
            _headers["set-cookie"] =
               "sessionId=;SameSite=None;Secure;HttpOnly;max-age=0";
               
         _headers["access-control-allow-origin"] =
            origin;
            
         _headers["access-control-allow-credentials"] =
            "true";
         

         _headers["cache-control"] =
            "no-store";
         
         if (_status == "200")
         {
            _headers["content-type"] =
               "application/json; charset=UTF-8";
       
            stringstream contentStream;
            
            contentStream 
               << "{" << endl
               << *this << endl
               << "}" << "\r\n";
               
            _serveFile = false;
            _content = contentStream.str();
            
         }
         else if ( !authenticated() &&
                   !isPrivileged(
                      request.path()
                    ) )
         {
            cerr << "HTTPS AUTH Redirect" << endl;
            redirect("/client/logon/", false);
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