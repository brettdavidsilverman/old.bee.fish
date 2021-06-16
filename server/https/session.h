#ifndef BEE_FISH_HTTPS__SESSION_H
#define BEE_FISH_HTTPS__SESSION_H

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
      string _tempFileName;
      std::fstream _tempFile;
      bool _isStillPosting = false;
      BString _exception;
      
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
         // Create the temp file name.
         stringstream stream;
         stream << this;
         _tempFileName =
            string(TEMP_DIRECTORY) +
            "/" +
            stream.str();
         
      }
  
  
      virtual ~Session()
      {
         clear();
      }
      
      virtual void start()
      {
         clear();
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
         
         if (_tempFile.is_open())
         {
            _tempFile.close();
         }
         
         if (filesystem::exists(_tempFileName))
            remove(_tempFileName);
         
         _isStillPosting = false;
         _exception = "";
         
      }

      virtual void handleRead(
         const boost::system::error_code& error,
         size_t bytesTransferred
      )
      {
      
         const string data =
            _data.substr(0, bytesTransferred);
         
         stringstream stream(data);
         
         if (!readRequest(stream))
         {
            delete this;
            return;
         }
         
            
         if ( _isStillPosting )
         {
            // More input to come...
            if (!_tempFile.is_open())
               openTempFile();
            _tempFile << data;
            asyncRead();
            return;
         }

         // All input is now in
         Server::writeDateTime(cout);
   
         cout
            << "\t"
            << ipAddress() << "\t"
            << _request->method()  << "\t"
            << _request->path()    << "\t"
            << _request->version()
            << std::endl;
         
         if (filesystem::exists(_tempFileName))
         {
            // Start reading from file
            delete _request;
   
            _request = new Request();
            ifstream input(_tempFileName);
            if (!readRequest(stream))
            {
               delete this;
               return;
            }
         }
         
         _response = new Response(
            this
         );
            
         if (!_response->end())
            asyncWrite();
         else
            start();
      }
      
      bool readRequest(istream& input)
      {
         // Parse the request
         _request->read(input);
         
         optional<bool> result =
            _request->_result;
            
         if (result == false)
         {
            
            // Parse error, drop the connection
            _log << "{\"parseError\": \"";
            _request->_character.writeEscaped(_log);
            _log << "\", "
                 << "\"byteCount\":" << _request->_byteCount
                 << "}"
                 << std::endl;
            _isStillPosting = false;
            return false;
         }
            

         if ( _request->method() == "POST" &&
               result == nullopt )
         {
            _isStillPosting = true;
         }
         else
            _isStillPosting = false;
            
         return true;

      }
      
      void openTempFile()
      {
         _tempFile.open(
            _tempFileName,
            std::fstream::in  |
            std::fstream::out |
            std::fstream::trunc
         );
         
         if (_tempFile.fail())
         {
            BString what = "Failed to open file: ";
            what += _tempFileName;
  
            logException(
               "Session::Start",
               what
            );
         }
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
         catch (const std::exception& ex)
         {
            logException("Session::asyncRead", ex.what());
            delete this;
            return;
         }

      }

      virtual void asyncWrite()
      {
         if (!_response || _response->end())
            start();
         
         size_t length = _maxLength;
         
         string data =
            _response->getNext(length);
            
         boost::asio::async_write(
            *this,
            boost::asio::buffer(
               data,
               length
            ),
            boost::bind(
               &Session::handleWrite,
               this,
               boost::asio::placeholders::error
            )
         );
      }

      virtual void logException(
         const BString& where,
         const BString& what
      )
      {
         stringstream stream;
         
         stream << "{\"exception\":{\"where\":\"";
         where.writeEscaped(stream);
         stream << "\",\"what\":\"";
         what.writeEscaped(stream);
         stream << "\",\"who\":\"" << this << "\"}}";
         
         _log << stream.str();
         
         cerr << endl << stream.str() << endl;
        
         _exception = stream.str();
 
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
      
      virtual void logException(
         const BString& where
      )
      {
         stringstream stream;
         stream << strerror(errno);
   
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
         catch (const std::exception& ex)
         {
            logException(
               "Session::ipAddress",
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
            logException(
               "Session::handleHandshake",
               error
            );
      
            delete this;
         }
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
  
      const BString& exception() const
      {
         return _exception;
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
   ) : Authentication(
          *( session->server()->database() ),
            session->ipAddress(),
            session->
               request()->
               getCookie("sessionId")
         )
   {
   }
      

}

#endif