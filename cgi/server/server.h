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
#include <database.h>
#include "session.h"

using namespace bee::fish::database;

namespace bee::fish::server {

class server
{
public:
   server(std::string host_name, boost::asio::io_context& io_context, unsigned short port);

   std::string get_password() const;
   std::string get_host_name() const;
   Database* get_database() const;
   
   void start_accept();

   void handle_accept(session* new_session,
       const boost::system::error_code& error);

private:
   boost::asio::io_context& _io_context;
   boost::asio::ip::tcp::acceptor _acceptor;
   boost::asio::ssl::context _context;
   std::string _host_name;
   Database* _database;

};

}

#endif