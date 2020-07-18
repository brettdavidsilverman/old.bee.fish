//
// session.cpp
// ~~~~~~~~~~
//
// Modified from
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <string>
#include <mcheck.h>

#include "session.h"
#include "request.h"
#include "response.h"

using namespace bee::fish::server;

session::session(
   boost::asio::io_context& io_context,
   boost::asio::ssl::context& ssl_context
) : ssl_socket(io_context, ssl_context)
{
   _max_length = getpagesize();
   _data = std::string(_max_length, 0);
}
  
session::~session() {

   if (_request) {
      delete _request;
      _request = NULL;
   }
   if (_response) {
      delete _response;
      _response = NULL;
   }
}


ssl_socket::lowest_layer_type&
session::socket()
{
   return lowest_layer();
}

void session::handshake()
{

   ssl_socket::async_handshake(
      boost::asio::ssl::stream_base::server,
      boost::bind(
         &session::handle_handshake,
         this,
         boost::asio::placeholders::error
      )
   );
}

void session::handle_handshake(
   const boost::system::error_code& error
)
{
   if (!error)
   {
      start();
   }
   else
   {
      delete this;
   }
}

void session::start() {

   clear();
      
   _request = new bee::fish::server::request();
   
   async_read();
}

void session::clear() {
   if (_request) {
      delete _request;
      _request = NULL;
   }
   if (_response) {
      delete _response;
      _response = NULL;
   }
}
   
void session::async_read() {

   async_read_some(
      boost::asio::buffer(
         _data,
         _max_length
      ),
      boost::bind(
         &session::handle_read,
         this,
         boost::asio::placeholders::error,
         boost::asio::placeholders::bytes_transferred
      )
   );

}

void session::handle_read(
   const boost::system::error_code& error,
   size_t bytes_transferred
)
{
   if (error) {
      delete this;
      return;
   }

   _request->read(
      _data.substr(
         0,
         bytes_transferred
      ),
      (bytes_transferred < _max_length)
   );

   optional<bool> success =
      _request->success();
        
   if (success == false) {
      // Parse error, drop the connection
      std::clog << ip_address()
                << " Fail!"
                << std::endl;
      delete this;
      return;
   }
   else if (success == nullopt) {
      // Continue reading
      async_read();
      return;
   }
   
   std::clog
      << std::endl
      << ip_address()
      << " "
      << "'" << _request->method() << "' "
      << "'" << _request->path() << "' "
      << "'" << _request->version() << "'"
      << std::endl;
  
   _response = new response(
      this,
      _request
   );

   if (_response->end())
      start();
   else
      async_write();
}

void session::async_write() {
 
   if (!_response || _response->end())
      start();
         
   string data =
      _response->write(_max_length);
         
   boost::asio::async_write(
      *this,
      boost::asio::buffer(
         data,
         data.length()
      ),
      boost::bind(
         &session::handle_write,
         this,
         boost::asio::placeholders::error
      )
   );
}

void session::handle_write(
   const boost::system::error_code& error
)
{
   if (error) {
      delete this;
      return;
   }
   
   if (_response->end()) {
      delete _response;
      _response = NULL;
      this->start();
   }
   else
      async_write();
}