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
#include "https_parser.h"

class https_session :
   public session,
   public https_parser
{
public:

   https_session(
      boost::asio::io_context& io_context,
      boost::asio::ssl::context& ssl_context
   );
   
   virtual void start();
   
   virtual void handle_read(
      const boost::system::error_code& error,
      size_t bytes_transferred
   );
    

};

#endif