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
         map<string, BString> headers;
         App* app = nullptr;
         
         for ( auto factory : appFactories )
         {

            app = factory->create(session);
            
            _status = app->status();

            merge(
               headers,
               app->headers()
            );
            
            
            if (_status != "")
               break;
               
            delete app;
            
            app = nullptr;
         }
         
         if (app)
         {
            cout << "Served by "
                 << app->name()
                 << " "
                 << _status
                 << endl;
                 
            if (app->serveFile())
            {
               _serveFile = true;
               _filePath = app->filePath();
               _contentLength = file_size(_filePath);
            }
            else
            {
               _serveFile = false;
               _content = app->content();
               _contentLength = _content.size();
            }
            
            delete app;
         
            headers["content-length"] =
               std::to_string(_contentLength);
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
               << pair.second
               << "\r\n";
         }
            
         headersStream << "\r\n";
         _headers = headersStream.str();
         _headersLength = _headers.size();
         
      }
      
      void merge(
         map<string, BString>& mapDest,
         const map<string, BString>& mapSrc
      )
      {
         for (auto pair : mapSrc)
         {
            mapDest[pair.first] = pair.second;
         }
         
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