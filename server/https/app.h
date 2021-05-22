#ifndef BEE_FISH_HTTPS__APP_H
#define BEE_FISH_HTTPS__APP_H
#include <vector>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

namespace bee::fish::https {

   class Session;
   
   class App {
   protected:
      Session* _session;
   protected:
      
      string _status;
      map<string, BString> _headers;
      string _content;
      bool   _serveFile;
      path   _filePath;

   public:
      App(
         Session* session
      ) : _session(session)
      {
      }
      
      virtual ~App()
      {
      }
      
      virtual string status()
      {
         return _status;
      }
      
      virtual const map<string, BString>& headers() const
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
         stringstream headerStream;
 
         if (permanent)
            _status = "301";
         else
            _status = "307";
            
         //_headers.clear();
        
         _headers["connection"] = "keep-alive";
         _headers["location"] = path;
  
         _content = "redirecting...";
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