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

class Server
{
public:
   Server( const std::wstring& hostName,
           boost::asio::io_context&
              ioContext,
           unsigned short port );

   std::string password() const;
   const std::wstring& hostName() const;
   Database* database() const;
   
   void startAccept();

   void handleAccept(
      Session* newSession,
      const boost::system::error_code&
         error
   );

private:
   std::wstring _hostName;
   boost::asio::io_context& _ioContext;
   boost::asio::ip::tcp::acceptor _acceptor;
   boost::asio::ssl::context _context;
   Database* _database;

};

}

#endif