#ifndef BEE_FISH_SERVER__SERVER_H
#define BEE_FISH_SERVER__SERVER_H

//
// server.h
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
#include "session.h"

namespace bee::fish::server {

class server
{
public:
   server(boost::asio::io_context& io_context, unsigned short port);

   std::string get_password() const;

   void start_accept();

   void handle_accept(session* new_session,
       const boost::system::error_code& error);

private:
   boost::asio::io_context& _io_context;
   boost::asio::ip::tcp::acceptor _acceptor;
   boost::asio::ssl::context _context;
};

}

#endif