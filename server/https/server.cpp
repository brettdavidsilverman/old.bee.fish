//
// server.cpp
// ~~~~~~~~~~
// 
// Modified fron
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.h"
#include "config.h"

using namespace bee::fish::server;

Server::Server(
   const std::wstring& hostName,
   boost::asio::io_context& ioContext,
   unsigned short port
) : _hostName(hostName),
    _ioContext(ioContext),
    _acceptor(
       ioContext,
       boost::asio::ip::tcp::endpoint(
          boost::asio::ip::tcp::v4(),
          port
       )
    ),
    _context(boost::asio::ssl::context::sslv23)
{

   _context.set_options(
      boost::asio::ssl::context::default_workarounds
      | boost::asio::ssl::context::no_sslv2
      | boost::asio::ssl::context::single_dh_use
   );
  
   // context_.set_password_callback(boost::bind(&server::get_password, this));
   _context.use_certificate_chain_file(CERT_FILE);
   _context.use_private_key_file(KEY_FILE, boost::asio::ssl::context::pem);
   _context.use_tmp_dh_file(TMP_DH_FILE);


   _database = new Database("bee.fish.data");
   
   startAccept();
}

std::string Server::password() const
{
   return "test";
}

Database* Server::database() const
{
   return _database;
}

void Server::startAccept()
{
   Session* newSession =
      new Session(
         this, 
        _ioContext,
        _context );
   
   _acceptor.async_accept(
      newSession->socket(),
      boost::bind(
        &Server::handleAccept,
        this,
        newSession,
        boost::asio::placeholders::error
      )
   );
}

void Server::handleAccept(
   Session* newSession,
   const boost::system::error_code& error
)
{
   if (!error)
   {
      newSession->handshake();
   }
   else
   {
      delete newSession;
   }

   startAccept();
}

const std::wstring& Server::hostName() const
{
   return _hostName;
}

