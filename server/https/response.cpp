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
   {
      cerr << "No Authorization header!" << endl;
   }
   
   std::ostringstream bodyStream;
   
   if ( auth && 
        auth->authenticated()
      )
   {
         
      
      if (request->hasBody())
      {
         
         Body& body   = request->body();
      
         if ( body.contains(L"method") &&
              body.contains(L"key") )
         {
            
            wstring& method = body.method();
            wstring& key    = body.key();

            Storage storage(*auth, request->path());
         
            if (method == L"getItem")
            {
                  
               if (storage.has(key))
               {
                  wstring value =
                     storage.getItem(key);
                     
                  bodyStream
                     << "\"";
                  
                  String::write(
                     bodyStream,
                     value
                  );
                  
                  bodyStream
                     << "\"";
                  
               }
               else
               {
                  bodyStream << "null";
               }
               
            
            }
            else if ( method == L"setItem" &&
                      body.contains(L"value") )
            {
               if (body.valueIsNull())
               {
                  storage.removeItem(key);
               }
               else
               {
                  wstring& value  = body.wvalue();
       
                  storage.setItem(key, value);
               }
               
               bodyStream
                  << "\"ok\"";
            }
            else if ( method == L"removeItem" )
            {
               storage.removeItem(key);
               bodyStream
                  << "\"ok\"";
            }
         }
      }
   }
   else
   {
      bodyStream
         << "\"Please log in.\"";
   }
   
   string body = bodyStream.str();
   cerr << body << endl;
   
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
      << "content-type: application/json\r\n"
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

