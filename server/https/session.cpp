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
#include <fstream>
#include "session.h"
#include "request.h"
#include "response.h"

using namespace bee::fish::server;

Session::Session(
   Server* server,
   boost::asio::io_context& ioContext,
   boost::asio::ssl::context& sslContext
) : SSLSocket(ioContext, sslContext)
{
   _server = server;
   _maxLength = getpagesize();
   _data = std::string(_maxLength, 0);
   _request = NULL;
   _response = NULL;
}
  
Session::~Session() {

   if (_request) {
      delete _request;
      _request = NULL;
   }
   if (_response) {
      delete _response;
      _response = NULL;
   }
}


SSLSocket::lowest_layer_type&
Session::socket()
{
   return lowest_layer();
}

void Session::handshake()
{

   SSLSocket::async_handshake(
      boost::asio::ssl::stream_base::server,
      boost::bind(
         &Session::handleHandshake,
         this,
         boost::asio::placeholders::error
      )
   );
}

void Session::handleHandshake(
   const boost::system::error_code& error
)
{
   if (!error)
   {
      start();
   }
   else
   {
      cerr << "Session::handleHandshake "
           << error
           << ", "
           << error.category().name()
           << endl;
      delete this;
   }
}

void Session::start() {

   clear();
      
   _request = new Request();
   
   asyncRead();
}

void Session::clear() {
   if (_request) {
      delete _request;
      _request = NULL;
   }
   if (_response) {
      delete _response;
      _response = NULL;
   }
}
   
void Session::asyncRead() {

   async_read_some(
      boost::asio::buffer(
         _data,
         _maxLength
      ),
      boost::bind(
         &Session::handleRead,
         this,
         boost::asio::placeholders::error,
         boost::asio::placeholders::bytes_transferred
      )
   );

}

void Session::handleRead(
   const boost::system::error_code& error,
   size_t bytesTransferred
)
{
   if (error) {
      delete this;
      return;
   }

#ifdef DEBUG
   // dump the data to a session.log file
   ofstream sessionLog(
      "session.log",
      ofstream::trunc | ofstream::binary
   );
   
   sessionLog << 
      _data.substr(0, bytesTransferred);
   
   sessionLog.close();
#endif

   _request->read(
      _data.substr(
         0,
         bytesTransferred
      ),
      false
   );

   optional<bool> success =
      _request->success();
        
   if (success == false) {
      // Parse error, drop the connection
      std::clog << std::endl
                << ipAddress()
                << "*********Fail!**********"
                << std::endl
                << _data.substr(0, bytesTransferred)
                << std::endl;
      delete this;
      return;
   }
   else if (success == nullopt) {
      // Continue reading
      asyncRead();
      return;
   }
   
   std::clog
      << ipAddress()
      << " "
      << _request->method() << " "
      << _request->path() << " "
      << _request->version() << " "
      << std::endl;
  
   _response = new Response(
      this
   );

   if (_response->end())
      start();
   else
      asyncWrite();
}

void Session::asyncWrite() {
 
   if (!_response || _response->end())
      start();
         
   string data =
      _response->write(_maxLength);
         
   boost::asio::async_write(
      *this,
      boost::asio::buffer(
         data,
         data.length()
      ),
      boost::bind(
         &Session::handleWrite,
         this,
         boost::asio::placeholders::error
      )
   );
}

void Session::handleWrite(
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
      asyncWrite();
}

Server* Session::server() const
{
   return _server;
}

Request* Session::request() const
{
   return _request;
}

Response* Session::response() const
{
   return _response;
}
