#ifndef BEE_FISH_SERVER__SESSION_H
#define BEE_FISH_SERVER__SESSION_H

//
// session.h
// ~~~~~~~~~~
//
// Modified from
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <unistd.h>
#include "request.h"

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SSLSocket;

namespace bee::fish::server {

class Request;
class Response;
class Server;

class Session : public SSLSocket
{
public:

   Session(
      Server* _server,
      boost::asio::io_context& ioContext,
      boost::asio::ssl::context& sslContext
   );
  
   virtual ~Session();

   SSLSocket::lowest_layer_type& socket();

   std::string ipAddress() const {
   
      return 
         lowest_layer()
            .remote_endpoint()
            .address()
            .to_string();
   }
   
   void handshake();

   void handleHandshake(const boost::system::error_code& error);
   
   virtual void start();
   
   virtual void clear();
   
   virtual void asyncRead();
  
   virtual void handleRead(
      const boost::system::error_code&
         error,
      size_t bytes_Transferred
   );

   virtual void asyncWrite();
   
   void handleWrite(
      const boost::system::error_code&
         error
   );
   
   Server* server();
   Request* request();
   Response* response();
   
protected:
   size_t _maxLength;
   std::string _data;
   Server* _server;
   Request* _request;
   Response* _response;
};

}

#endif