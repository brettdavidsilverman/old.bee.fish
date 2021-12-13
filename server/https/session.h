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

namespace BeeFishHTTPS {

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
      Parser* _parser;
      Response* _response;
      string _tempFileName;
      std::fstream _tempFile;
      bool _isStillPosting = false;
      BString _exception;
      BString _ipAddress;
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
         _parser(nullptr),
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
         
         if (filesystem::exists(_tempFileName))
            remove(_tempFileName);

         _server->startAccept();
      }
      
      virtual void start()
      {
         clear();
         
         try
         {
            _ipAddress =  
               lowest_layer()
                  .remote_endpoint()
                  .address()
                  .to_string();
         }
         catch (...)
         {
            _ipAddress = "";
            logException("Session::start", "Invalid ipAddress");
            delete this;
            return;
         }
         _request = new Request();
         _parser = new Parser(*_request);
         asyncRead();
      }
   
      virtual void clear()
      {
         if (_request)
         {
            delete _request;
            _request = nullptr;
         }
         
         if (_parser)
         {
            delete _parser;
            _parser = nullptr;
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
         
         _exception = "";
         
      }

      virtual void handleRead(
         const boost::system::error_code& error,
         size_t bytesTransferred
      )
      {
         if (bytesTransferred == 0) {
            delete this;
            return;
         }
         
         if (error)
         {
            logException("handleRead", error);
            delete this;
            return;
         }
         
         const string data =
            _data.substr(0, bytesTransferred);
         _parser->read(data);
         
         if (_request->result() == false)
         {
            delete this;
            return;
         }
         
            
         if (_request->method() == "GET")
         {
            handleResponse();
            return;
         }

         // Write current session data to file
         if (!_tempFile.is_open())
         {
            try
            {
               openTempFile();
            }
            catch (...)
            {
               delete this;
               return;
            }
          
         }
         _tempFile << data;
         
         // Check if finished request
         if (_request->result() == true)
         {
            _tempFile.close();
            
            _server->appendToLogFile(_tempFileName);

            // Authenticate from existing request
            //??Authentication auth(this);
/*
            _request = new Request();
            _parser = new Parser(*_request);
            ifstream input(_tempFileName);

            if (_parser->read(input) == false)
            {
               delete this;
               return;
            }
*/
            handleResponse();
            return;
         }
         
         // More data to come...
         asyncRead();
      }
      


      void handleResponse() 
      {
         try {
            // All input is now in
            Server::writeDateTime(clog);
      
            clog
               << "\t"
               << ipAddress() << "\t"
               << _request->method()  << "\t"
               << _request->path()    << "\t"
               << _request->query() << "\t"
               << _request->version()
               << std::endl;
            
            _response = new Response(
               this
            );

            if (!_response->end())
               asyncWrite();
            else {
               if (_request->headers()["connection"] == "close") {
                  delete this;
                  return;
               }
               start();
            }

         }
         catch (std::exception& ex) {
            logException("Session::handleResponse", ex.what());
            delete this;
            return;
         }
         catch (...)
         {
            logException("Session::handleResponse", "Unkown error");
            delete this;
            return;
         }
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
            
            throw runtime_error("Failed to open temp file");
         }
         
         permissions(
            _tempFileName,
               perms::group_read  |
               perms::group_write |
               perms::others_read |
               perms::others_write,
            perm_options::remove
         );
         
         permissions(
            _tempFileName,
            perms::owner_read |
               perms::owner_write,
            perm_options::replace
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
      
      virtual void asyncRead()
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
         ostream& stream = cerr;
         
         stream << "{"
                << endl
                << "   \"exception\": {"
                << endl
                << "      \"where\": \"";
         where.writeEscaped(stream);
         stream << "\","
                << endl
                << "      \"ipAddress\": \""
                << ipAddress();
         stream << "\","
                << endl
                << "      \"what\": \"";
         what.writeEscaped(stream);
         stream << "\","
                << endl
                << "      \"who\": \""
                << this << "\"";
         stream << "\","
                << endl
                << "      \"when\": \"";
         Server::writeDateTime(stream);
         stream << "\""
                << endl
                << "   }"
                << endl
                << "}"
                << endl;
      }

   
      virtual void logException(
         const BString& where,
         const boost::system::error_code& error
      )
      {
      
         if (error.value() == 1)
            // session connection truncated
            return;
            
         stringstream stream;
         stream << error.category().name() << ":" << error.value() << ":" << error.message();
   
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

      const BString& ipAddress() const
      {
         return _ipAddress;
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
         start();
         return;

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

      void setRequest(Request* request) {
         if (_request)
            delete _request;
         _request = request;
      }
      
      Response* response()
      {
         return _response;
      }
  
      string tempFileName() {
         return this->_tempFileName;
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

   // Defined in app.h
   inline bool App::parseRequest(
      JSONParser& parser
   )
   {
      
      ifstream input(_session->tempFileName());
      Request* request = (Request*)(parser.getMatch());
      parser.read(input);

      input.close();

      if (parser.result() == true) {
         _session->setRequest(request);
         return true;
      }
      else {
         return false;
         delete request;
      }

      throw std::logic_error("Should not reach here in session.h");      
   }

   inline Request* App::request() {
      return _session->request();
   }

}

#endif