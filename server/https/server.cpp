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
#include <boost/asio/ssl/context.hpp>

#include "server.h"
#include "config.h"

using namespace bee::fish::server;

std::string my_password_callback(
    std::size_t max_length,  // the maximum length for a password
    boost::asio::ssl::context::password_purpose purpose ) // for_reading or for_writing
{
    cerr << "my_password_callback" << endl;
    std::string password = "test"; 
    // security warning: !! DO NOT hard-code the password here !!
    // read it from a SECURE location on your system
    return password;
}

std::string Server::password()
{
   std::cerr << "getting *** password...";
   return std::string("test");
}
Server::Server(
   const std::wstring& hostName,
   const std::string databaseFile,
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
   std::cerr << "Starting server...";
   /*
   _context.set_options(
      boost::asio::ssl::context::default_workarounds
      | boost::asio::ssl::context::no_sslv2
      | boost::asio::ssl::context::single_dh_use
   );
   */
   std::cerr << "setting up passwords...";
  
   _context.set_password_callback(
      my_password_callback
   );
   _context.use_certificate_chain_file(CERT_FILE);
   
   try
   {
      _context.use_private_key_file(KEY_FILE, boost::asio::ssl::context::file_format::pem);
   }
   catch (boost::system::system_error& ex)
   {
      error_code ec = ex.code();
      std::cerr << ec.value() << std::endl;
      std::cerr << ec.category().name() << std::endl;
      throw ex;
   }
  // _context.use_tmp_dh_file(TMP_DH_FILE);

   std::cerr << "setting up database...";
   _database = new Database(databaseFile);
   
   std::cerr << "start accept...";
   startAccept();
   std::cerr << "ok" << std::endl;
}



Database* Server::database()
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

