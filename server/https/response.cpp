#include <boost/algorithm/string/predicate.hpp>
#include <mcheck.h>
#include <iostream>
#include <iomanip>
#include "config.h"
#include "basic-authorization.h"
#include "request.h"
#include "token.h"
#include "session.h"
#include "response.h"
#include "wstring.h"

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
      
   Token* token = NULL;
  
   
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
         token = new Token(
            server,
            session->ipAddress(),
            basicAuth.username(),
            basicAuth.password()
         );
      }
     
      basicAuth.password().clear();
   }
   else if (request->hasBody())
   {
      Object& json =
         (Object&)
            (request->body().item());
            
      if (json.contains(L"hash") &&
          json.contains(L"username"))
      {
      
         string& hash =
            json[L"hash"].value();
            
         wstring& username =
            json[L"username"].wvalue();
            
         // Authenticate using hash and username
         token = new Token(
            server,
            session->ipAddress(),
            hash,
            username
         );
      }
   }
  
   std::ostringstream bodyStream;
   
   if (token && token->authenticated())
   {
      bodyStream
         << *token
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
   
   if (token && token->authenticated())
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
   
   if (token)
      delete token;
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

