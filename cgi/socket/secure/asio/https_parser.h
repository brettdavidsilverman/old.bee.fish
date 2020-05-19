#ifndef HTTPS_PARSER_H
#define HTTPS_PARSER_H
//
// https_parser.h
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
#include "reader_writer.h"


class https_parser :
   public reader
{
public:

   https_parser()
   {
      clear();
   }
   
   virtual void read(char c) {
      next_reader_(c);
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
      header
   };
   
   const std::vector<reader_function> 
      readers_ =
   {
      // Method
      [this](char c) {
         if (c == ' ')
            next_reader_ = readers_[path];
         else
            method_ += c;
      },
      // Path
      [this](char c) {
         if (c == '?')
            next_reader_ = readers_[query];
         else if (c == ' ')
            next_reader_ = readers_[version];
         else
            path_ += c;
               
      },
      // Query
      [this](char c) {
         if (c == ' ')
            next_reader_ =
                  readers_[version];
         else
            query_ += c;
      },
      // Version
      [this](char c) {
         if (c == '\r' || c == '\n')
            next_reader_ =
                  readers_[header];
         else
            version_ += c;
      },
      // Header
      [this](char c) {
         
         return;
         
         if (c == '\r' || c == '\n')
            return;
         if (c == ' ')
            next_reader_ =
                  readers_[header];
      }
   };
   
   reader_function next_reader_;
      
};

#endif