#ifndef BEE_FISH_HTTPS__APP_H
#define BEE_FISH_HTTPS__APP_H
#include <vector>
#include <filesystem>
#include "response-headers.h"
#include "authentication.h"

using namespace std;
using namespace std::filesystem;

namespace bee::fish::https {

   class Session;
   
   typedef ResponseHeaders Headers;
   
   class App : public Authentication {
   protected:
      Session* _session;
   protected:
      
      string _status;
      Headers& _headers;
      string _content;
      bool   _serveFile;
      path   _filePath;

   public:
      App(
         Session* session,
         Headers& headers
      ) :
         Authentication(session),
         _session(session),
         _headers(headers)
      {
      }
      
      virtual ~App()
      {
      }
      
      virtual string status()
      {
         return _status;
      }
      
      virtual const Headers& headers() const
      {
         return _headers;
      }
      
      virtual string content()
      {
         return _content;
      }
      
      virtual bool serveFile()
      {
         return _serveFile;
      }
      
      virtual path filePath()
      {
         return _filePath;
      }
      
      virtual BString name()
      {
         return "App base class";
      }
      
      void redirect(
         BString path,
         bool permanent
      )
      {
         if (permanent)
            _status = "301";
         else
            _status = "307";
            
         _headers.replace(
            "connection", "keep-alive"
         );
         
         _headers.replace(
            "location", path
         );
         
         _headers.replace(
            "cache-control",
            "no-store, no-cache, must-revalidate"
         );
         
         _content = BString("redirecting...");
         
         _serveFile = false;
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
      
      virtual App* create(
         Session* session,
         Headers& headers
      ) = 0;
      
   };
   
   template<class T>
   class AppFactory : public Factory
   {
   public:
      AppFactory()
      {
      }
      
      virtual App* create(Session* session, Headers& headers)
      {
         return new T(session, headers);
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