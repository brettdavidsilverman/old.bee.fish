//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server.h"

server::server(
   boost::asio::io_context& io_context,
   unsigned short port
) : io_context_(io_context),
    acceptor_(
       io_context,
       boost::asio::ip::tcp::endpoint(
          boost::asio::ip::tcp::v4(),
          port
       )
    ),
    context_(boost::asio::ssl::context::sslv23)
{

   context_.set_options(
      boost::asio::ssl::context::default_workarounds
      | boost::asio::ssl::context::no_sslv2
      | boost::asio::ssl::context::single_dh_use
   );
  
   // context_.set_password_callback(boost::bind(&server::get_password, this));
   context_.use_certificate_chain_file("/etc/letsencrypt/live/bee.fish/fullchain.pem");
   context_.use_private_key_file("/etc/letsencrypt/live/bee.fish/privkey.pem", boost::asio::ssl::context::pem);
   context_.use_tmp_dh_file("dh2048.pem");

   start_accept();
}

std::string server::get_password() const
{
      return "test";
}

void server::start_accept()
{
   session* new_session =
      new https_session(io_context_, context_);
   
   acceptor_.async_accept(
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


