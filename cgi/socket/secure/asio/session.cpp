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

session::session(
   boost::asio::io_context& io_context,
   boost::asio::ssl::context& ssl_context
) : ssl_socket(io_context, ssl_context),
   max_length_(getpagesize()),
   data_(max_length_, 0)
{

   std::cout << "session()" << std::endl;
   
}
  
session::~session() {

   std::cout << "~session()" << std::endl;
   
}


ssl_socket::lowest_layer_type&
session::socket()
{
   return lowest_layer();
}

void session::handshake()
{
   std::cout << "session::handshake()" << std::endl;
   
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

void session::handle_read(
   const boost::system::error_code& error,
   size_t bytes_transferred
)
{
   if (!error)
   {

      boost::asio::async_write(
         *this,
         boost::asio::buffer(
            data_,
            bytes_transferred
         ),
         boost::bind(
            &session::handle_write,
            this,
            boost::asio::placeholders::error
         )
      );
      
   }
   else
   {
      delete this;
   }
}

void session::handle_write(
   const boost::system::error_code& error
)
{

    if (!error)
    {
       async_read_some(
          boost::asio::buffer(
             data_,
             max_length_
          ),
          boost::bind(
             &session::handle_read,
             this,
             boost::asio::placeholders::error,
             boost::asio::placeholders::bytes_transferred
          )
       );
    }
    else
    {
       delete this;
    }
}