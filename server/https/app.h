#ifndef BEE_FISH_SERVER__APP_H
#define BEE_FISH_SERVER__APP_H
#include <vector>
#include "config.h"
#include "session.h"
#include "request.h"

using namespace std;

namespace bee::fish::https {

   class App {
   protected:
      Session* _session;
      string _response;
   private:
      size_t _bytesTransferred = 0;
   public:
      App(
         Session* session
      ) : _session(session)
      {
      }
      
      virtual string getNext(size_t length)
      {
     
         if (_bytesTransferred + length >
            _response.length())
            length = _response.length() -
               _bytesTransferred;
            
         string retval =
            _response.substr(
               _bytesTransferred,
               length
            );
         
         _bytesTransferred += length;
      
         return retval;
      }
   
   
      virtual bool end()
      {
         return _bytesTransferred >=
         _response.length();
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