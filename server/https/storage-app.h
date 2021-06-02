#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include <optional>

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

         Authentication auth(
            session
         );

         if (!auth)
            return;
            
         std::ostringstream headerStream;
      
         _JSON& json =
            *( request._json );
      
         _Object& object =
            *( json._object );
            
         Storage storage(auth, "test");//request.path());
         
         optional<BString> method = nullopt;
         optional<BString> key = nullopt;
         optional<BString> value = nullopt;

         bool returnValue = false;
         bool returnJSON = true;
         
         // Get the method
         if ( object.contains("method") )
         {
            
            method =
               object["method"]->value();
         }
            
         // Get the key
         if ( request.method() == "POST" &&
              object.contains("key") )
         {
            key =
               object["key"]->value();
         }
         else if ( request.method() == "GET" )
         {
            key = getKeyFromPath(
                  request
               );

            if (key != nullopt)
            {
               method = "getItem";
               returnJSON = false;
            }
         }
                  
         // Get the value
         if ( object.contains("value") )
         {
            if ( object["value"]->isNull() )
               value = nullopt;
            else
               value =
                  object["value"]->value();
         }
         
         // Execute the method
         
         // Get item
         if ( method == "getItem" &&
              key != nullopt )
         {
            returnValue = true;
               
            if (storage.has(key.value()))
            {
               value =
                  storage.getItem(key.value());
            }
            else
               value = nullopt;
               
            _status = "200";
         }
         // Set item
         else if ( method == "setItem" &&
                   key != nullopt )
         {
            if ( value == nullopt )
            {
               storage.removeItem(
                  key.value()
               );
            }
            else
            {
               storage.setItem(
                  key.value(),
                  value.value()
               );
            }
                     
            _status = "200";
               
         }
         // Remove item
         else if ( method == "removeItem" &&
                      key != nullopt )
         {
            storage.removeItem(key.value());
            _status = "200";
         }
         // Clear
         else if (method == "clear")
         {
            key = nullopt;
                     
            storage.clear();
            _status = "200";
         }
               
         if ( _status != "200" )
            return;
            
         if ( !returnJSON )
         {
            _headers["content-type"] =
               "text/plain; charset=UTF-8";
            if ( value != nullopt )
               _content = value.value();
            else
               _content = "";
               
            _serveFile = false;
            
            return;
         }
         
         std::ostringstream contentStream;
   
         contentStream << "{" << endl;
         
         if ( key != nullopt )
         {
            contentStream
               << "   \"key\":\"";
                     
            key.value()
              .writeEscaped(contentStream);
        
            contentStream
               << "\"";
         }
         else
         {
            contentStream
               << "   \"key\":null";
         }
               
         contentStream
            << "," << endl
            << "   \"response\":\"ok\"";
                  
         if (returnValue)
         {
            contentStream
               << "," << endl
               << "   \"value\":";
            
            if (value == nullopt)
               contentStream << "null";
            else
            {
               contentStream << 
                  value.value();
            }
         
         }
   
         contentStream << endl << "}";
   
         _headers["content-type"] =
            "application/json; charset=UTF-8";
            
         _content = contentStream.str();
         _serveFile = false;
         
         cerr << _content << endl;
   
      }
      
      optional<BString> getKeyFromPath(
         const Request& request
      )
      {
         if ( request.path() !=
              "/client/storage/" )
         {
            return nullopt;
         }
            
         const BString& query =
            request.query();
         
         if ( query.size() )
         {
            return query.substr(1);
         }
         
         return nullopt;
      }
   
      virtual BString name()
      {
         return "Storage app";
      }

   };

   

}

#endif
