#ifndef BEE_FISH_HTTPS__RESPONSE_H
#define BEE_FISH_HTTPS__RESPONSE_H

#include <filesystem>

#include "app.h"

using namespace std;
using namespace std::filesystem;

namespace BeeFishHTTPS {

   class Session;
   
   class Response {
   protected:
      const bool _log = true;

      Session* _session;
      
      int _status;
      string _statusText;

      string _headers;
      
      size_t _headersLength = 0;
      size_t _bytesTransferred = 0;
      size_t _contentLength = 0;
      
      App*      _app = nullptr;
   public:
      Response(
         Session* session
      )
      {
         
         ResponseHeaders headers;

         App* app = nullptr;


         for ( auto factory : appFactories )
         {

            app = factory->create(
               session,
               headers
            );
            
            app->handleResponse();
            
            _status = app->status();
            _statusText = app->statusText();

            if (_status != -1)
               break;
               
            
            delete app;
            
            app = nullptr;

         }

         if (app)
         {

            if (_log) {
               clog << _status
                  << " " << _statusText
                  << " Served by "
                  << app->name();
            }
                 
            
            if (_log)
               clog << endl;
            
            _app = app;

            if (_app->serveFile()) {
               _contentLength = file_size(_app->_filePath);
            }
            else if (_app->serveContent()) {
               _contentLength = _app->_content.size();
            }
            else if (_app->serveData()) {
               _contentLength = _app->_data.size();
            }
            else {
               _contentLength = _app->_contentLength;
            }
         
            headers.replace(
               "content-length",
               std::to_string(_contentLength)
            );
         }

         stringstream headersStream;
         
         headersStream 
            << "HTTP/1.1 " 
            << _status 
            << " "
            << _statusText
            << "\r\n";
            
         for (auto pair : headers)
         {
            headersStream
               << pair.first
               << ": "
               << pair.second.str()
               << "\r\n";
         }
            
         headersStream << "\r\n";
         _headers = headersStream.str();
         _headersLength = _headers.size();

         DEBUG_OUT("Sending headers");
         DEBUG_OUT("\r\n");
         DEBUG_OUT(_headers);
         DEBUG_OUT("\r\n");

       
      }
      
      virtual ~Response()
      {
      }
      
      virtual Data getNext(size_t& length)
      { 
         
         if (_bytesTransferred < _headersLength)
         {
            std::cout << "SERVING HEADERS" << std::endl;

            // Serve headers
            if ( (_bytesTransferred + length) 
                 > _headersLength )
            {
               length =
                  _headersLength -
                  _bytesTransferred;
            }
            
/*            
            string response =
               _headers.substr(
                  _bytesTransferred,
                  length
               );
*/            
            std::cout << "SENDING HEADERS LENGTH: " << length << std::endl;

            Data response = 
               Data(
                  (const Byte*)(_headers.data() + _bytesTransferred), 
                  length
               );

            _bytesTransferred += length;
            
            return response;
         }
         
         Data response;
         
         // Calculate largest length (truncating if required)
         if ( ( _bytesTransferred + length ) >
              (_headersLength + _contentLength ) )
            length =
               _headersLength +
               _contentLength -
               _bytesTransferred;
               
         if (_app->serveData()) {
            std::cout << "SERVING DATA" << std::endl;
            Data chunk = Data(_app->_data.data() + (_bytesTransferred - _headersLength), length);
            response = chunk;
               
         }
         else if ( _app->serveFile() )
         {
            std::cout << "SERVING FILE" << std::endl;
            Data buffer = Data::create(length);

            ifstream input(_app->_filePath);
               input.seekg(
                  _bytesTransferred -
                  _headersLength
               );
            
            input.read((char*)buffer.data(), length);
            
            input.close();
         
            response = buffer;   
         }
         else
         {
            std::cout << "SERVING CONTENT" << std::endl;

            response =
               Data(
                  (const Byte*)(_app->_content.data()) +
                     _bytesTransferred -
                     _headersLength,
                  length
               );
         }
         
         _bytesTransferred += length;

         std::cout << "RETURNING RESPONSE" << std::endl;
         
         return response;
      }
   
      virtual bool end() 
      {
         bool end =
            _bytesTransferred ==
            ( _headersLength + _contentLength );
            
         if ( end )
         {
            _headers.clear();

            if (_app) {
               delete _app;
               _app = nullptr;

            }
         }
         
         return end;
      }

   

   };
   

   
}

#endif