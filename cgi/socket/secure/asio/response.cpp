#include "response.h"
#include <boost/algorithm/string/predicate.hpp>
#include <mcheck.h>
#include <iostream>
#include <iomanip>

using namespace bee::fish::server;
using namespace boost::algorithm;


response::response(
   bee::fish::server::session* session,
   bee::fish::server::request* request
)
{
   
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
/*
   if (headers->count(
         "authorization"
      ) > 0)
   {
      Header*
         authorization = 
          //  (BasicAuthorizationHeader*)
               (*headers)["authorization"];
 
      if (authorization) {
         body_stream
            << authorization->base64()
            << "\r\n";
      }
   }
      */
   for (auto it = headers.cbegin();
             it != headers.cend();
           ++it)
   {
      AbstractHeader* header = it->second;
      body_stream
         << header->name()
         << '\t'
         << header->value()
         << "\r\n";
   }
      
   
   string body = body_stream.str();
   
   std::ostringstream out;
   
   out
     //"HTTP/1.1 200 OK\r\n";
      << "HTTP/1.1 401 Unauthorized\r\n"
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

