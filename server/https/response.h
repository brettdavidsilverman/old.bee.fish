#ifndef BEE_FISH_HTTPS__RESPONSE_H
#define BEE_FISH_HTTPS__RESPONSE_H

#include <filesystem>

#include "app.h"

using namespace std;
using namespace std::filesystem;

namespace bee::fish::https {

   class Session;
   
   class Response {
   protected:
      Session* _session;
      
      string _status;
      string _headers;
      
      path   _filePath;
      bool   _serveFile;

      string _content;
      
      size_t _headersLength = 0;
      size_t _contentLength = 0;
      size_t _bytesTransferred = 0;

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
            
            _status = app->status();
            
            
            if (_status != "")
               break;
               
            delete app;
            
            app = nullptr;
         }
         
         if (app)
         {
            cout << BString(_status)
                 << " Served by "
                 << app->name();
                 
            if (app->serveFile())
            {
               _serveFile = true;
               _filePath = app->filePath();
               _contentLength = file_size(_filePath);
               cout << ": " << _filePath << endl;
            }
            else
            {
               _serveFile = false;
               _content = app->content();
               _contentLength = _content.size();
            }
            
            cout << endl;
            
            delete app;
         
            headers.replace(
               "content-length",
               std::to_string(_contentLength)
            );
         }

         stringstream headersStream;
         
         headersStream 
            << "HTTP/1.1 " 
            << _status 
            << " OK\r\n";
            
         for (auto pair : headers)
         {
            headersStream
               << pair.first
               << ": "
               << pair.second.toUTF8()
               << "\r\n";
         }
            
         headersStream << "\r\n";
         _headers = headersStream.str();
         _headersLength = _headers.size();
        
      }
      
      virtual ~Response()
      {
      }
      
      virtual string getNext(size_t& length)
      { 
         
         if (_bytesTransferred < _headersLength)
         {
            // Serve headers
            if ( (_bytesTransferred + length) 
                 > _headersLength )
            {
               length =
                  _headersLength -
                  _bytesTransferred;
            }
            
            
            string response =
               _headers.substr(
                  _bytesTransferred,
                  length
               );
            
            _bytesTransferred += length;
#ifdef DEBUG
            cerr << response;
#endif
            return response;
            
         }
         
         _headers.clear();
         
         string response;
         
         if ( ( _bytesTransferred + length ) >
              (_headersLength + _contentLength ) )
            length =
               _headersLength +
               _contentLength -
               _bytesTransferred;
               
         if ( _serveFile )
         {
            char buffer[length];

            ifstream input(_filePath);
               input.seekg(
                  _bytesTransferred -
                  _headersLength
               );
               input.read(buffer, length);
            input.close();
         
            response = string(buffer, length);
         }
         else
         {
            response = _content.substr(
               _bytesTransferred -
               _headersLength,
               length
            );
#ifdef DEBUG
            cerr << response;
#endif
         }
         
         _bytesTransferred += length;
         

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
            _content.clear();

         }
         
         return end;
      }

   

   };
   

   
}

#endif