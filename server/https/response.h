#ifndef BEE_FISH_SERVER__RESPONSE_H
#define BEE_FISH_SERVER__RESPONSE_H
#include "config.h"
#include "session.h"
#include "request.h"
#include "app.h"

using namespace std;

namespace bee::fish::https {

   class Response {
   protected:
      Session* _session;
      App* _app;
   public:
      // Declared in app.h
      Response(
         Session* session
      )
      {
         _app =
            appFactories[0]->create(session);
      }
      
      string getNext(size_t& length)
      {
         return _app->getNext(length);
      }
   
   
      bool end()
      {
         return _app->end();
      }
   

   };
   

   
}

#endif