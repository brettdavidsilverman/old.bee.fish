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

server::server(
   std::string host_name,
   boost::asio::io_context& io_context,
   unsigned short port
) : _host_name(host_name),
    _io_context(io_context),
    _acceptor(
       io_context,
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


   _database = new Database("bee.fish.db2");
   _database->pointer = 0;
   
   start_accept();
}

std::string server::get_password() const
{
      return "test";
}

Database* server::get_database() const
{
   return _database;
}

void server::start_accept()
{
   session* new_session =
      new session(this, _io_context, _context);
   
   _acceptor.async_accept(
      new_session->socket(),
      boost::bind(
        &server::handle_accept,
        this,
        new_session,
        boost::asio::placeholders::error
      )
   );
}

void server::handle_accept(
   session* new_session,
   const boost::system::error_code& error
)
{
   if (!error)
   {
      new_session->handshake();
   }
   else
   {
      delete new_session;
   }

   start_accept();
}

std::string server::get_host_name() const
{
   return _host_name;
}

