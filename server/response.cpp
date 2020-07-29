#include <boost/algorithm/string/predicate.hpp>
#include <mcheck.h>
#include <iostream>
#include <iomanip>
#include "basic-authorization.h"
#include "request.h"
#include "session.h"
#include "response.h"

using namespace bee::fish::server;
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
   std::ostringstream bodyStream;
   
      
   Headers& headers =
      request->headers();
      
   Token token(server);
   
   if (headers.contains("authorization"))
   {
      Header* header =
         headers["authorization"];
         
      BasicAuthorization basicAuth(
         header->value()
      );
      
      header->value().clear();
      
      if (basicAuth.success() == true) {
         token = Token(
            server,
            session->ipAddress(),
            basicAuth.email(),
            basicAuth.password()
         );
      }
     
      basicAuth.password().clear();
    
      if (token.authenticated()) {
         bodyStream
            << token
            << "\r\n"
            << "\r\n";
      }
      else {
         bodyStream
            << "Unauthenticated"
            << "\r\n";
      }
         
   }
   
   string body = bodyStream.str();
   
   cerr << body << endl;
   string origin;
   
   if (headers["origin"])
      origin = headers["origin"]->value();
   else if (headers["host"])
      origin = headers["host"]->value();
   else
      origin = "bee.fish";
      
   cerr << origin << endl;
   std::ostringstream out;
   
   if (token.authenticated())
      out << "HTTP/1.1 200 OK\r\n";
   else
      out << "HTTP/1.1 401 Unauthorized\r\n";
   out
      << "content-type: text/plain\r\n"
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

