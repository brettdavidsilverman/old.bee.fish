#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H
#include "config.h"
#include "session.h"
#include "request.h"
#include "response.h"
#include "authentication.h"
#include "storage.h"
#include "app.h"

using namespace std;

namespace bee::fish::https {

   class StorageApp : public App {
   public:
      StorageApp(
         Session* session
      ) : App(session)
      {
   
         Request* request = session->request();
         const string& path = request->path();
   
         std::ostringstream out;
      
         Authentication auth(
            session
         );

         std::ostringstream bodyStream;
   
         bodyStream << "{" << endl;
         bodyStream << auth;
      
         if ( auth &&
              request->method() == "POST" &&
              request->hasBody() )
         {
         
            Request::Body& body   = request->body();
      
      
      
            if ( body.contains("method") )
            {
            
               const BString& method = body.method();
            
               BString value;
               bool valueIsNull = true;
               bool returnValue = false;
               Storage storage(auth, "test");//request->path());
         
         
         
               if (body.contains("key"))
               {
                  const BString& key = body.key();
            
                  if (method == "getItem")
                  {
                     returnValue = true;
               
                     if (storage.has(key))
                     {
                        value =
                           storage.getItem(key);            
                        valueIsNull = false;
                     }
               
            
                  }
                  else if ( method == "setItem" &&
                         body.contains("value") )
                  {
                     if (body.valueIsNull())
                     {
                        storage.removeItem(key);
                     }
                     else
                     {
                        const BString& value =
                           body.value();
       
                        storage.setItem(
                           key, value
                        );
                     }
               
                  }
                  else if ( method == "removeItem" )
                  {
                     storage.removeItem(key);

                  }
               
                  bodyStream
                     << ",\"key\":\"";
                  key.writeEscaped(bodyStream);
                  bodyStream
                     << "\"";
               }
            
               if (method == "clear")
               {
                  storage.clear();
               }
            
               bodyStream
                  << ",\"response\":\"ok\"";
                  
               if (returnValue)
               {
                  bodyStream
                     << ",\"value\":";
                  if (valueIsNull)
                     bodyStream << "null";
                  else
                  {
                     bodyStream
                        << "\"";
                     value.writeEscaped(bodyStream);
                     bodyStream
                        << "\"";
                  }
               }
            
            }
         
         }
   
         bodyStream << endl << "}";
   
         string body = bodyStream.str();
/*
         cerr << "********" << endl;
         cerr << body << endl;
         cerr << "********" << endl;
*/
         string origin;
   
         const Request::Headers& headers =
            request->headers();
      
         if (headers.contains("origin"))
            origin = headers["origin"];
         else if (headers.contains("host"))
            origin = headers["host"];
         else
            origin = HOST_NAME;
   

         out
            << "HTTP/1.1 200 OK\r\n"
            << "content-type: application/json; charset=UTF-8\r\n"
            << "content-length: "
            << std::to_string(body.size()) 
               << "\r\n"
            << "connection: keep-alive\r\n";
      
         if (auth)
            out
               << "set-cookie: sessionId=" << auth.sessionId() << ";SameSite=None;Secure;HttpOnly\r\n";
         else
            out
               << "set-cookie: sessionId=" << "x" << ";SameSite=None;Secure;HttpOnly;max-age=0\r\n";
         out
            << "Access-Control-Allow-Origin: "
               << origin
               << "\r\n"
            << "Access-Control-Allow-Credentials: "
               << "true\r\n"
            << "\r\n";
            
         _content = body;
         _contentLength = _content.size();
         _headers = out.str();
         _headersLength = _headers.size();
         _serveFile = false;
   

      }
   

   };

   

}

#endif
