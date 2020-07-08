#ifndef HTTPS_SESSION_H
#define HTTPS_SESSION_H
//
// https_session.h
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
#include <vector>
#include <parser.h>
#include "session.h"
#include "http-request.h"

class https_session :
   public session
{
public:
   
   https_session(
      boost::asio::io_context& io_context,
      boost::asio::ssl::context& ssl_context
   ) :
      session(io_context, ssl_context)
   {
      std::cout << "https_session()" << std::endl;
      clear();
   }
   
   virtual ~https_session() {
      if (http_request) {
         delete http_request;
         http_request = NULL;
      }
   }
   
   virtual void start() {
      std::cout << "start()" << std::endl;
   
      https_session::clear();
      http_request = new Bee::Fish::http_request();

      async_read_some();
   }

   void clear() {
      if (http_request)
         delete http_request;
      http_request = NULL;
   }
   
   void async_read_some() {
      session::async_read_some(
         boost::asio::buffer(
            _data,
            _max_length
         ),
         boost::bind(
            &https_session::handle_read,
            this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
      );
   }
   
   virtual void handle_read(
      const boost::system::error_code& error,
      size_t bytes_transferred
   );
   
   Bee::Fish::http_request* http_request = NULL;
      
};

#endif