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
   std::string header_label_;
   std::string buffer_;
   std::map<std::string, std::string> headers_;
   
private:
   
   enum {
      method,
      path,
      query,
      version,
      crlf,
      header_label,
      header_value,
      crlf_crlf
   };
   
   // Carriage Return/Line Feed
   class crlf_reader :
      public reader
   {
   private:
      bool first = true;;
      
   public:
      crlf_reader()
      : reader(
         [this](char c) {
            std::cout << "crlf: " << c << std::endl;
            if (first) {
               if (c == '\r') {
                  first = false;
                  return true;
               }
            }
         
            if (c == '\n') {
               std::cout << "crlf" << std::endl;
               return true;
            }
            return false;
         }
      )
      {
         std::cout << "crlf_reader()" << std::endl;
      }
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
      
         std::cout << "v: " << c << std::endl;
         
         if (c == '\r') {
            buffer_ = "\r";
            return true;
         }
         
         if (buffer_ == "\r" && c == '\n') {
            buffer_ = "";
            std::cout << "er" << std::endl;
            next_reader_ = either_reader(
               readers_[crlf],
               readers_[header_label]
            );
         }
         else
            version_ += c;
            
         return true;
      },
      // crlf
      crlf_reader(),
      // Header label
      [this](char c) {
         std::cout << "hl: " + c << std::endl;
         
         if (c == ':') {
            header_label_ = buffer_;
            buffer_ = "";
            next_reader_ =
                  readers_[header_value];
            return true;
         }
         
         buffer_ += c;
         return true;
      },
      // Header value
      [this](char c) {
         
         if (c == '\r' || c == '\n') {
            headers_[header_label_] = buffer_;
            next_reader_ = either_reader(
                readers_[header_label],
                readers_[crlf_crlf]
            );
            return false;
         }
         
         buffer_ += c;
            
         return true;
      },
      [this] (char c) {
         // Double crlf
         next_reader_ = sequenced_reader(
            readers_[crlf],
            sequenced_reader(
               readers_[crlf],
               [this] (char c) {
                  std::cout << "end!" << std::endl;
                  return false;
               }
            )
         );
         return next_reader_(c);
      }
   };
      
   reader_function next_reader_;
      
};

#endif