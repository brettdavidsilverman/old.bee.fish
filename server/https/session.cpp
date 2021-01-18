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
#include "session.h"
#include "request.h"
#include "response.h"
#include "server.h"

using namespace bee::fish::server;

Session::Session(
   Server* server,
   boost::asio::io_context& ioContext,
   boost::asio::ssl::context& sslContext
) : SSLSocket(ioContext, sslContext),
    _log(server->log())
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
      
   _log
      << std::endl
      << "*******New Request*******"
      << this
      << std::endl;

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

   // dump the data to a session.log file
   _log 
      << std::endl
      << "***************"
      << this
      << std::endl;
      
   _log << 
      _data.substr(0, bytesTransferred);
   _log << std::endl << "####" << bytesTransferred << std::endl;

   _request->read(
      _data.substr(
         0,
         bytesTransferred
      ),
      false
   );

   optional<bool> result =
      _request->result();
        
   if (result == false) {
      // Parse error, drop the connection
      _log
         << std::endl
         << ipAddress()
         << std::endl
         << "*********Fail!**********"
         << std::endl
         << _data.substr(0, bytesTransferred)
         << std::endl
         << "************************"
         << std::endl;
      delete this;
      return;
   }

   long contentLength =
      _request->contentLength();
         
   if (contentLength > 0)
   {
      long currentContentLength = 0;
      
      if (_request->hasBody())
         currentContentLength =
            _request->body().contentLength();
         
      _log
         << "$$$ ContentLength: " 
         << contentLength
         << ", "
         << "CurrentContentLength: "
         << currentContentLength
         << std::endl;
   
      if ( currentContentLength <
           contentLength )
      {
         asyncRead();
         return;
      }
   }
   
   cout
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

Server* Session::server()
{
   return _server;
}

Request* Session::request()
{
   return _request;
}

Response* Session::response()
{
   return _response;
}
