#ifndef BEE_FISH_SERVER__SESSION_H
#define BEE_FISH_SERVER__SESSION_H

//
// session.h
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
#include <fstream>
#include "server.h"
#include "request.h"
#include "response.h"
#include "authentication.h"


typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SSLSocket;

namespace bee::fish::https {

   class Request;
   class Response;
   class Server;
 
   class Session : public SSLSocket
   {
   protected:
      Server* _server;
      std::ofstream& _log;
      size_t _maxLength;
      std::string _data;
      
      Request* _request;
      Response* _response;
      
   public:

      Session(
         Server* server,
         boost::asio::io_context& ioContext,
         boost::asio::ssl::context& sslContext
      ) :
         SSLSocket(ioContext, sslContext),
         _server(server),
         _log(server->log()),
         _maxLength(getpagesize()),
         _data(string(_maxLength, 0)),
         _request(nullptr),
         _response(nullptr)
      {

      }
  
  
      virtual ~Session()
      {
         _log << "{\"message\":\"End session\"" << ", "
              << "\"session\":\"" << this << "\""
              << "}" 
              << std::endl;
        
         if (_request)
         {
            delete _request;
            _request = nullptr;
         }
         
         if (_response)
         {
            delete _response;
            _response = nullptr;
         }
   
      }

      virtual void logException(
         const BString& where,
         const BString& what
      )
      {
         _log << "{\"exception\":{\"where\":\"";
         where.writeEscaped(_log);
         _log << "\",\"what\":\"";
         what.writeEscaped(_log);
         _log << "\",\"who\":\"" << this << "\"}}";
      }

   
      virtual void logException(
         const BString& where,
         const boost::system::error_code& error
      )
      {
         stringstream stream;
         stream << error;
   
         logException(where, stream.str());
      }
   
      SSLSocket::lowest_layer_type& socket()
      {
         return lowest_layer();
      }

      BString ipAddress()
      {
         try
         {
            return 
               lowest_layer()
               .remote_endpoint()
               .address()
               .to_string();
         }
         catch (const exception& ex)
         {
            logException(
               "Session::ipAdress",
               ex.what()
            );
            return "";
         }
      }
   
      void handshake()
      {
         SSLSocket::async_handshake(
            boost::asio::ssl::stream_base::server,
            boost::bind(
               &Session::handleHandshake,
               this,
               boost::asio::placeholders::error
            )
         );
      }


      void handleHandshake(const boost::system::error_code& error)
      {
         if (!error)
         {
            start();
         }
         else
         {
            cerr << "Session::handleHandshake "
                 << error
                 << ", "
                 << error.category().name()
                 << endl;
           
            logException(
               "Session::handleHandshake",
               error
            );
      
            delete this;
         }
      }
      
      virtual void start()
      {
         clear();
      
         _log 
            << "{"
            << "\"message\":\"New session\"" << ", "
            << "\"session\":\"" << this << "\", "
            << "\"ipAddress\":\"" << ipAddress() << "\", "
            << "\"time\": \"";
         Server::writeTime(_log);
         _log
            << "\""
            << "}"
            << std::endl;

         _request = new Request();
   
         asyncRead();
      }
   
      virtual void clear()
      {
         if (_request)
         {
            delete _request;
            _request = nullptr;
         }
         
         if (_response)
         {
            delete _response;
            _response = nullptr;
         }
      }
      
      virtual void asyncRead()
      {

         try
         {
            async_read_some(
               boost::asio::buffer(
                  _data,
                  _maxLength
               ),
               boost::bind(
                  &Session::handleRead,
                  this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred
               )
            );
         }
         catch (const exception& ex)
         {
            logException("Session::asyncRead", ex.what());
            delete this;
            return;
         }

      }

  
      virtual void handleRead(
         const boost::system::error_code& error,
         size_t bytesTransferred
      )
      {

         _log 
            << "{"
            << "\"message\":\"Handle read\"" << ", "
            << "\"session\":\"" << this << "\", "
            << "\"bytes\":" << bytesTransferred << ", "
            << "\"ipAddress\":\"" << ipAddress() << "\"";
      
         if (error)
            _log << ", "
                 << "\"error\":\"" << error << "\"";
           
         _log
            << "}"
            << std::endl;
      
         const string data =
            _data.substr(0, bytesTransferred);
  
         _log << data << std::endl;
         
         if (error)
         {
            logException(
               "Session::handleRead",
               error
            );
            delete this;
            return;
         }
   
 
         
     
         // Parse the request
         _request->read(data, false);
   
         optional<bool> result =
            _request->_result;
        
         cerr << endl << "!!!!!!" << result << "!!!!" << endl;
         
         if (result == false)
         {
            // Parse error, drop the connection
            _log << "{\"error\": \"";
            _request->_character.writeEscaped(_log);
            _log << "\"}"
                 << std::endl;
              
            delete this;
            return;
         }
         else if (result == nullopt)
         {
            asyncRead();
            return;
         }
   
         Server::writeTime(cout);
   
         cout
            << "\t"
            << ipAddress() << "\t"
            << _request->method()  << "\t"
            << _request->path()    << "\t"
            << _request->version()
            << std::endl;

         try
         {
            _response = new Response(
               this
            );
         }
         catch (exception& ex)
         {
   
            logException(
               "Session::handleRead",
               ex.what()
            );
      
            cerr << "********Bee.Fish HTTPS Error*******" 
                 << endl
                 << ex.what()
                 << endl;
           
            delete this;
            return;
         }

         if (_response->end())
            start();
         else
            asyncWrite();
      }


      virtual void asyncWrite()
      {
 
         if (!_response || _response->end())
            start();
         
         string data =
            _response->getNext(_maxLength);
         
         boost::asio::async_write(
            *this,
            boost::asio::buffer(
               data,
               data.length()
            ),
            boost::bind(
               &Session::handleWrite,
               this,
               boost::asio::placeholders::error
            )
         );
      }
   
      void handleWrite(
         const boost::system::error_code&
            error
      )
      {
         if (error)
         {
            logException(
               "Session::handleWrite",
               error
            );
            delete this;
            return;
         }
   
         if (_response->end()) 
         {
            delete _response;
            _response = nullptr;
            start();
         }
         else
            asyncWrite();
      }

   
      Server* server()
      {
         return _server;
      }
      
      Request* request()
      {
         return _request;
      }
      
      Response* response()
      {
         return _response;
      }
  
   };
   
   // Declared in server.h
   inline void Server::startAccept()
   {
      Session* newSession =
         new Session(
            this, 
           _ioContext,
           _context
        );
   
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
   
   // Declared in server.h
   inline void Server::handleAccept(
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

   // Declared in authentication.h
   inline Authentication::Authentication(
      Session* session
   ) :
      Authentication(
         *( session->server()->database() ),
         *( session->request() ),
         session->ipAddress()
      )
   {
   }
      

}

#endif