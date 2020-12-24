#include <boost/algorithm/string/predicate.hpp>
#include <iostream>
#include <iomanip>
#include "config.h"
#include "basic-authorization.h"
#include "request.h"
#include "authentication.h"
#include "session.h"
#include "response.h"
#include "wstring.h"
#include "storage.h"

using namespace bee::fish::server;
using namespace bee::fish::parser::json;
using namespace boost::algorithm;


Response::Response(
   Session* session
)
{
   
   Request* request = session->request();
   Server* server = session->server();
   // const string& path = request->path();
   /*
   if (ends_with(path, "/?mtrace")) {
      cout << "*mtrace*" << endl;
      mtrace();
   }
      
   if (ends_with(path, "?muntrace")) {
      cout << "*muntrace*" << endl;
      muntrace();
   }
   */
   
      
   Headers& headers =
      request->headers();
      
   Authentication* auth = NULL;
  
   
   if (headers.contains("authorization"))
   {
      cerr << "Authorization header" << endl;
      
      std::string& header =
         headers["authorization"];
      
      BasicAuthorization basicAuth(
         header
      );
      
      header.clear();
      if (basicAuth.success() == true) {
         // Authenticate using username
         // and password
         auth = new Authentication(
            server,
            session->ipAddress(),
            basicAuth.username(),
            basicAuth.password()
         );
      }
     
      basicAuth.password().clear();
   }
   else
      cerr << "No Authorization header!" << endl;
   
   std::ostringstream bodyStream;
   
   if ( auth && 
        auth->authenticated()
      )
   {
      bodyStream
         << "{";
         
      
      if (request->hasBody())
      {
         
         Body& body   = request->body();
      
         if ( body.contains(L"method") &&
              body.contains(L"key") )
         {
            string& method = body.method();
            string& key    = body.key();
         

            Storage storage(*auth, request->path());
         
            if (method == "getItem")
            {
               string value = storage.getItem(key);
               bodyStream
                  << "\"response\": {\"key\":\"";
               String::write(bodyStream, key);
               bodyStream
                  << "\",\"value\":\"";
               String::write(bodyStream, value);
               bodyStream
                  << "\"}";
            
            }
            else if ( method == "setItem" &&
                      body.contains(L"value") )
            {
               string& value  = body.value();
               storage.setItem(key, value);
               bodyStream
                  << "\"response\": \"ok\"";
            }
         }
      }
      bodyStream
         << "}"
         << "\r\n"
         << "\r\n";
   }
   else
   {
      bodyStream
         << "{"
         << "\"message\": "
         << "\"Please log in\""
         << "}"
         << "\r\n"
         << "\r\n";
   }
   
   string body = bodyStream.str();
   
   string origin;
   
   if (headers.contains("origin"))
      origin = headers["origin"];
   else if (headers.contains("host"))
      origin = headers["host"];
   else
      origin = ws2s(HOST_NAME);
      
   std::ostringstream out;
   
   if (auth && auth->authenticated())
      out << "HTTP/1.1 200 OK\r\n";
   else
      out << "HTTP/1.1 401 Unauthorized\r\n";
   out
      << "content-type: text/json\r\n"
      << "content-length: "
      << std::to_string(body.length()) 
         << "\r\n"
      << "connection: keep-alive\r\n"
      << "Access-Control-Allow-Origin: "
         << origin
         << "\r\n"
      << "Access-Control-Allow-Credentials: "
         << "true\r\n"
      << "WWW-Authenticate: "
         << "Basic realm="
         << "\"bee.fish\"" << "\r\n"
      << "\r\n"
      << body;
      
   _response = out.str();
   
   if (auth)
      delete auth;
}

string Response::write(size_t length) {
   
   if (_bytesTransferred + length >
       _response.length())
      length = _response.length() -
         _bytesTransferred;
            
   string retval =
      _response.substr(
         _bytesTransferred,
         length
      );
         
   _bytesTransferred += length;
      
   return retval;
}

bool Response::end() {
   return
      _bytesTransferred >=
      _response.length();
}

