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
#include "session.h"
#include "reader_writer.h"


class https_session :
   public session,
   public reader
{
public:
   
   https_session(
      boost::asio::io_context& io_context,
      boost::asio::ssl::context& ssl_context
   ) :
      session(io_context, ssl_context)
      //next_reader_(null_reader)
   {
      std::cout << "https_session()" << std::endl;
      clear();
   }
   
   virtual void start() {
      std::cout << "start()" << std::endl;
   
      https_session::clear();
   
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

   
   virtual bool read(char c) {
      return next_reader_(c);
   }

   void clear() {
      method_ = "";
      path_ = "";
      query_ = "";
      version_ = "";
      buffer_ = "";
      headers_.clear();
      next_reader_ = readers_[method];
   }
   
   virtual void handle_read(
      const boost::system::error_code& error,
      size_t bytes_transferred
   );
   
   
protected:
   std::string method_;
   std::string path_;
   std::string query_;
   std::string version_;
   std::string buffer_;
   std::map<std::string, std::string> headers_;
   
private:
   
   enum {
      method,
      path,
      query,
      version,
      crlf,
      header
   };
   
   std::vector<reader_function> 
      readers_ =
   {
      // Method
      [this](char c) {
         if (c == ' ')
            next_reader_ = readers_[path];
         else
            method_ += c;
            
         return true;
      },
      // Path
      [this](char c) {
         if (c == '?')
            next_reader_ = readers_[query];
         else if (c == ' ')
            next_reader_ = readers_[version];
         else
            path_ += c;
         
         return true;
               
      },
      // Query
      [this](char c) {
         if (c == ' ')
            next_reader_ =
                  readers_[version];
         else
            query_ += c;
         return true;
      },
      // Version
      [this](char c) {
      
         if (c == '\r' || c == '\n') {
            
            next_reader_ = either_reader(
               next_reader(
                  readers_[crlf],
                  readers_[header]
               ),
               next_reader(
                  readers_[crlf],
                  readers_[crlf]
               )
            );
            
         }
         else
            version_ += c;
            
         return true;
      },
      // crlf
      [this](char c) {
         if (c == '\r') {
            buffer_ = "\r";
            return true;
         }
         
         if (buffer_ == "\r" && c == '\n')
            return true;
         
         return false;
      },
      // Header
      [this](char c) {
         
         if (c == '\r' || c == '\n')
            return true;
         if (c == ' ')
            next_reader_ =
                  readers_[header];
         return false;
      }
   };
      
   reader_function next_reader_;
      
};

#endif