#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H
#include "config.h"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"


using namespace std;

namespace bee::fish::https {

   class StorageApp : public App {
   public:
      StorageApp(
         Session* session
      ) : App(session)
      {
   
         Request& request =
            *( session->request() );
            
         const string& path = request.path();
   
      
         Authentication auth(
            session
         );

         std::ostringstream headerStream;
         std::ostringstream contentStream;
   
         contentStream << "{" << endl;
      
         if ( auth &&
              request.method() == "POST" &&
              request.hasJSON() )
         {
         
            _JSON& json =
               *( request._json );
      
            _Object& object =
               *( json._object );
      
            if ( object.contains("method") )
            {
            
               const BString& method =
                  object["method"]->value();
               
               cerr << "storage.app.Method : "
                    << method
                    << endl;
               
               BString value;
               bool valueIsNull = true;
               bool returnValue = false;
               Storage storage(auth, "test");//request.path());
         
         
         
               if (object.contains("key"))
               {
                  const BString& key =
                     object["key"]->value(); 
            
                  if (method == "getItem")
                  {
                     returnValue = true;
               
                     if (storage.has(key))
                     {
                        value =
                           storage.getItem(key);            
                        valueIsNull = false;
                     }
               
                     _status = "200";
                  }
                  else if ( method == "setItem" &&
                         object.contains("value") )
                  {
                     if ( object["value"]->isNull() )
                     {
                        storage.removeItem(key);
                     }
                     else
                     {
                        const BString& value =
                           object["value"]->value();
       
                        storage.setItem(
                           key, value
                        );
                     }
                     
                     _status = "200";
               
                  }
                  else if ( method == "removeItem" )
                  {
                     storage.removeItem(key);
                     _status = "200";
                  }
               
                  contentStream
                     << "\"key\":\"";
                  key.writeEscaped(contentStream);
                  contentStream
                     << "\"";
               }
            
               if (method == "clear")
               {
                  contentStream
                     << "\"key\":null";
                     
                  storage.clear();
                  _status = "200";
               }
            
               contentStream
                  << ",\"response\":\"ok\"";
                  
               if (returnValue)
               {
                  contentStream
                     << ",\"value\":";
                  if (valueIsNull)
                     contentStream << "null";
                  else
                  {
                     contentStream
                        << "\"";
                     value.writeEscaped(contentStream);
                     contentStream
                        << "\"";
                  }
               }
            
            }
         
         }
   
         contentStream << endl << "}";
   
         _content = contentStream.str();
         _serveFile = false;
   

      }
   

   };

   

}

#endif
