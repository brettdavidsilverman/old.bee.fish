#include <boost/algorithm/string/predicate.hpp>
#include <iostream>
#include <iomanip>
#include "config.h"
#include "request.h"
#include "authentication.h"
#include "session.h"
#include "response.h"
#include "../database/storage.h"

using namespace bee::fish::https;
using namespace bee::fish::json;
using namespace boost::algorithm;
using namespace bee::fish::database;

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
      
   Request::Headers& headers =
      request->headers();
      
  // cerr << headers << endl;
   
   Authentication auth(
      *session
   );
  

   
   
   if (headers.contains("cookie"))
   {
      BString& cookies = headers["cookie"];
      //cerr << "Cookies: " << cookies << endl;
      
   }
   /*
   {
      BasicAuthorization basicAuth(
         header
      );
      
      header.clear();
      if (basicAuth.result() == true) {
         // Authenticate using username
         // and password
         auth = new Authentication(
            server,
            session->ipAddress(),
            basicAuth.credentials()
         );
      }
     
   }
   */
   
   std::ostringstream bodyStream;
   
   if ( auth )
   {
         /*
      
      if (request->hasBody())
      {
         
         Request::Body& body   = request->body();
      
         cerr << body << endl;
         
         if ( body.contains("method") )
         {
            
            const BString& method = body.method();
            
            BString value;
            bool valueIsNull = true;
            bool returnValue = false;
            Storage storage(auth, "test");//request->path());
            cerr << method << endl;
            bodyStream << "{";
            
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
                  << "\"key\":\""
                  << key
                  << "\",";
            }
            
            if (method == "clear")
            {
               storage.clear();
            }
            
            bodyStream
                  << "\"response\":\"ok\"";
                  
            if (returnValue)
            {
               bodyStream
                  << ",\"value\":";
               if (valueIsNull)
                  bodyStream << "null";
               else
               {
                  bodyStream
                     << "\""
                     << value
                     << "\"";
               }
            }
            
            bodyStream << "}";
            
         }
         
      }
      */
   }
   else
   {
      bodyStream
         << "\"Please log in.\"";
   }
   
  // BString body = bodyStream.str();

   stringstream stream;
   stream << auth;
   BString body = stream.str();
   
#ifdef DEBUG
   cerr << body << endl;
#endif

   BString origin;
   
   if (headers.contains("origin"))
      origin = headers["origin"];
   else if (headers.contains("host"))
      origin = headers["host"];
   else
      origin = HOST_NAME;
      
   std::ostringstream out;
   
   if (auth)
      out << "HTTP/1.1 200 OK\r\n";
   else
      out << "HTTP/1.1 401 Unauthorized\r\n";
   out
     // << "allow: OPTIONS, GET, HEAD, POST\r\n"
      << "content-type: application/json; charset=UTF-8\r\n"
      << "content-length: "
      << std::to_string(body.size()) 
         << "\r\n"
      << "connection: keep-alive\r\n"
      << "set-cookie: bee.fish=helloWorld\r\n"
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

