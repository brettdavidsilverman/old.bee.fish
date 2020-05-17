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
#include <list>
#include "session.h"
#include "reader_writer.h"

class https_session :
   public session,
   public reader_writer
{
public:

   https_session(
      boost::asio::io_context& io_context,
      boost::asio::ssl::context& ssl_context
   );
   
   virtual void start();
   
   void parse_request(const std::string& buff);
   
   virtual void handle_read(
      const boost::system::error_code& error,
      size_t bytes_transferred
   );
   
   virtual char read();
   virtual void write(char c);
       
private:
   typedef std::function<void(char c)> writer_;
   writer_ next_writer_;
   
   std::string method_;
   std::string path_;
   std::string query_;
   std::string version_;
   std::string buffer_;
   
   std::map<std::string, writer_> writers_ = {
      {
         "method",
         [this](char c) {
            if (c == ' ')
               next_writer_ =
                  writers_["path"];
            else
               method_ += c;
         }
      },
      {
         "path",
         [this](char c) {
            if (c == '?')
               next_writer_ =
                  writers_["query"];
            else if (c == ' ')
               next_writer_ =
                  writers_["version"];
            else
               path_ += c;
               
         }
      },
      {
         "query",
         [this](char c) {
            if (c == ' ')
               next_writer_ =
                  writers_["version"];
            else
               query_ += c;
         }
      },
      {
         "version",
         [this](char c) {
            if (c == '\r')
               buffer_ = "\r";
            else if (c == '\n')
               buffer_ += 'c';
            if (buffer_ == "\r\n") {
               buffer_ = "";
               next_writer_ =
                  writers_["headers"];
            }
            else
               version_ += c;
         }
      },
      {
         "headers",
         [this](char c) {
            
         }
         
      }
   };
   
};

#endif