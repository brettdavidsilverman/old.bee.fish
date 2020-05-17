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
) : session(io_context, ssl_context)
{
   std::cout << "https_session()" << std::endl;
}
   
void https_session::start() {
   std::cout << "start()" << std::endl;
   method_ = "";
   path_ = "";
   query_ = "";
   version_ = "";
   next_writer_ = writers_["method"];
   
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
  
      reader_writer::write(
         data_.substr(
            0,
            bytes_transferred
         )
      );
      
      std::cout
         << "'" << method_ << "' "
         << "'" << path_ << "' "
         << "'" << query_ << "' "
         << "'" << version_ << "'"
         << std::endl;
      
      std::string response =
         "HTTP/1.1 200 OK\r\n";
      response +=
         "content-type: text/plain\r\n";
      response +=
         "content-length: " +
            std::to_string(bytes_transferred) +
            "\r\n";
      response += 
         "connection: keep-alive\r\n";
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

void https_session::write(char c) {
   next_writer_(c);
}

char https_session::read()
{
   return '\0';
}

void https_session::parse_request(const std::string& data) {


}