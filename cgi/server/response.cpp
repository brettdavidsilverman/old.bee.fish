#include "response.h"
#include <boost/algorithm/string/predicate.hpp>
#include <mcheck.h>
#include <iostream>
#include <iomanip>
#include "basic-authorization.h"

using namespace bee::fish::server;
using namespace boost::algorithm;


response::response(
   session* session
)
{
   
   request* request = session->get_request();
   server* server = session->get_server();
   string path = request->path();
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
   std::ostringstream body_stream;
   
   body_stream
      << session->ip_address()
      << "\r\n";
    
   Headers& headers =
      request->headers();

   Token token;
   
   if (headers.contains("authorization"))
   {
      Header* header =
         headers["authorization"];
         
      BasicAuthorization basicAuth(
         header->value()
      );
      
      if (basicAuth.success() == true) {
         token = Token(
            server,
            session->ip_address(),
            basicAuth.email(),
            basicAuth.password()
         );
      }
        
      if (token.authenticated()) {
         body_stream
            << token
            << "\r\n";
      }
      else {
         body_stream
            << "Unauthenticated"
            << "\r\n";
      }
         
   }
   
   for (auto it = headers.cbegin();
             it != headers.cend();
           ++it)
   {

      Header* header = it->second;
      body_stream
         << header->name()
         << '\t'
         << header->value()
         << "\r\n";
   }
      
   
   string body = body_stream.str();
   
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
         << "http://localhost:8000\r\n"
      << "Access-Control-Allow-Credentials: "
         << "true\r\n"
      << "WWW-Authenticate: "
         << "Basic realm="
            << "\"bee.fish\"" << "\r\n"
      //_response +=
      //   "WWW-Authenticate: Digest realm=\"bee@bee.fish\"\r\n";
      << "\r\n"
      << body;
      
   _response = out.str();
}

string response::write(size_t length) {
   
   if (_bytes_transferred + length >
       _response.length())
      length = _response.length() -
         _bytes_transferred;
            
   string retval =
      _response.substr(
         _bytes_transferred,
         length
      );
         
   _bytes_transferred += length;
      
   return retval;
}

bool response::end() {
   return
      _bytes_transferred >=
      _response.length();
}

