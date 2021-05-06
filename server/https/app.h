#ifndef BEE_FISH_SERVER__APP_H
#define BEE_FISH_SERVER__APP_H
#include <vector>
#include <filesystem>

#include "config.h"
#include "session.h"
#include "request.h"

using namespace std;
using namespace std::filesystem;

namespace bee::fish::https {

   class App {
   protected:
      Session* _session;
      path   _filePath;
      bool   _serveFile;
      
      string _headers;
      string _content;
      
      size_t _headersLength = 0;
      size_t _contentLength = 0;
      size_t _bytesTransferred = 0;
   public:
      App(
         Session* session
      ) : _session(session)
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
            //cerr << response;
            
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
   
   
   class Factory
   {
   public:
      Factory()
      {
      }
      
      virtual ~Factory()
      {
      }
      
      virtual App* create(Session* session) = 0;
      
   };
   
   template<class T>
   class AppFactory : public Factory
   {
   public:
      AppFactory()
      {
      }
      
      virtual App* create(Session* session)
      {
         return new T(session);
      }
   };
   
   class AppFactories :
      public vector<Factory*>
   {
   public:
      template<class T>
      void add()
      {
         push_back(new AppFactory<T>());
      }
      
      ~AppFactories()
      {
         for ( auto it = begin();
                    it != end();
                    ++it )
         {
            Factory* factory = *it;
            delete factory;
         }
      }
      
   };
   
   inline AppFactories appFactories;
   

}

#endif