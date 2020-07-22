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

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

namespace bee::fish::server {

class request;
class response;
class server;

class session : public ssl_socket
{
public:

   session(
      bee::fish::server::server* _server,
      boost::asio::io_context& io_context,
      boost::asio::ssl::context& ssl_context
   );
  
   virtual ~session();

   ssl_socket::lowest_layer_type& socket();

   std::string ip_address() const {
   
      return 
         lowest_layer()
            .remote_endpoint()
            .address()
            .to_string();
   }
   
   void handshake();

   void handle_handshake(const boost::system::error_code& error);
   
   virtual void start();
   
   virtual void clear();
   
   virtual void async_read();
  
   virtual void handle_read(const boost::system::error_code& error,
       size_t bytes_transferred);

   virtual void async_write();
   
   void handle_write(const boost::system::error_code& error);
   
   bee::fish::server::server* get_server() const;
   request* get_request() const;
   response* get_response() const;
   
protected:
   size_t _max_length;
   std::string _data;
   bee::fish::server::server* _server;
   request* _request;
   response* _response;
};

}

#endif