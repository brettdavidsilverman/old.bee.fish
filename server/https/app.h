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
   
   class App : public Authentication {
   protected:
      Session* _session;
   protected:
      
      string _status;
      ResponseHeaders& _responseHeaders;
      string _content;
      bool   _serveFile = false;
      path   _filePath;

   public:
      App(
         Session* session,
         ResponseHeaders& responseHeaders
      ) :
         Authentication(session),
         _session(session),
         _responseHeaders(responseHeaders)
      {
      }
      
      virtual ~App()
      {
      }
      
      virtual string status()
      {
         return _status;
      }
      
      virtual const ResponseHeaders& responseHeaders() const
      {
         return _responseHeaders;
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
         bool permanent,
         BString from = ""
      )
      {
         if (permanent)
            _status = "301";
         else
            _status = "307";
            
         _responseHeaders.replace(
            "connection", "keep-alive"
         );
         
         _responseHeaders.replace(
            "location", path
         );
         
         if (!permanent) {
            _responseHeaders.replace(
               "cache-control",
               "no-store, no-cache, must-revalidate, post-check=0, pre-check=0, expires=Sat, 26 Jul 1990 05:00:00 GMT"
            );
         }
         
         _responseHeaders.replace(
            "content-type",
            "text/plain; charset=UTF-8"
         );
         
         if (path != from)
         {
            _responseHeaders.emplace(
               "set-cookie",
               BString("redirect=;path=/;max-age=0")
            
            );
            _responseHeaders.emplace(
               "set-cookie",
               BString("redirect=") + from +
               ";path=/"
            );
         }
         
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
         ResponseHeaders& headers
      ) = 0;
      
   };
   
   template<class T>
   class AppFactory : public Factory
   {
   public:
      AppFactory()
      {
      }
      
      virtual App* create(Session* session, ResponseHeaders& headers)
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