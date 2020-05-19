//
// https_session.cpp
// ~~~~~~~~~~
//
// Modified from
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "https_session.h"

https_session::https_session(
   boost::asio::io_context& io_context,
   boost::asio::ssl::context& ssl_context
) :
   session(io_context, ssl_context)
{
   std::cout << "https_session()" << std::endl;
}
   
void https_session::start() {
   std::cout << "start()" << std::endl;
   
   https_parser::clear();
   
   async_read_some(
      boost::asio::buffer(
         data_,
         max_length_
      ),
      boost::bind(
         &https_session::handle_read,
         this,
         boost::asio::placeholders::error,
         boost::asio::placeholders::bytes_transferred
      )
   );
}

void https_session::handle_read(
   const boost::system::error_code& error,
   size_t bytes_transferred
)
{
   if (!error) {
  
      std::cout << "handle_read()" << std::endl;
      
      reader::read(
         data_.substr(
            0,
            bytes_transferred
         )
      );
      
      std::cout
         << "'" << https_parser::method_ << "' "
         << "'" << https_parser::path_ << "' "
         << "'" << https_parser::query_ << "' "
         << "'" << https_parser::version_ << "'"
         << std::endl;
      
      std::string response =
        "HTTP/1.1 200 OK\r\n";
        // "HTTP/1.1 401 Unauthorized\r\n";
      response +=
         "content-type: text/plain\r\n";
      response +=
         "content-length: " +
            std::to_string(bytes_transferred) +
            "\r\n";
      response += 
         "connection: keep-alive\r\n";
      response +=
         "WWW-Authenticate: Digest realm=\"bee@bee.fish\"\r\n";
      response += 
         "\r\n";
     
      boost::asio::write(
         *this,
         boost::asio::buffer(
            response,
            response.length()
         )
      );
        
      
      boost::asio::async_write(
         *this,
         boost::asio::buffer(
            data_,
            bytes_transferred
         ),
         boost::bind(
            &https_session::start,
            this
         )
      );
   }
   else
      delete this;
}

