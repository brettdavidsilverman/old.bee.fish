#ifndef BEE_FISH_SERVER__FILE_SYSTEM_APP_H
#define BEE_FISH_SERVER__FILE_SYSTEM_APP_H

#include "config.h"
#include "session.h"
#include "request.h"
#include "response.h"
#include "authentication.h"
#include "app.h"

using namespace std;

namespace bee::fish::https {

   class FileSystemApp : public App {
   public:
      FileSystemApp(
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
   
         bodyStream << "{";
         bodyStream << auth;
         bodyStream << "}";
   
         string body = bodyStream.str();

         cerr << "********" << endl;
         cerr << body << endl;
         cerr << "********" << endl;

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
         // << "allow: OPTIONS, GET, HEAD, POST\r\n"
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
               << "set-cookie: sessionId=" << "x" << ";SameSite=None;Secure;HttpOnly;max-age=-1\r\n";
         out
            << "Access-Control-Allow-Origin: "
               << origin
               << "\r\n"
            << "Access-Control-Allow-Credentials: "
               << "true\r\n"
            << "\r\n"
            << body;
      
         _response = out.str();
   

      }
   

   };

   

}

#endif
