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
         std::ostringstream contentStream;
   
         contentStream << "{" << endl;
      
         _JSON& json =
            *( request._json );
      
         _Object& object =
            *( json._object );
            
         Storage storage(auth, "test");//request.path());
         
         optional<BString> method = nullopt;
         optional<BString> key = nullopt;
         optional<BString> value = nullopt;

         bool returnValue = false;
         
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
                  request.path()
               );

            if (key != nullopt)
               method = "getItem";
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
            
         if ( key != nullopt )
         {
            contentStream
               << "\"key\":\"";
                     
            key.value()
              .writeEscaped(contentStream);
        
            contentStream
               << "\"";
         }
         else
         {
            contentStream
               << "\"key\":null";
         }
               
         contentStream
            << ",\"response\":\"ok\"";
                  
         if (returnValue)
         {
            contentStream
               << ",\"value\":";
            
            if (value == nullopt)
               contentStream << "null";
            else
            {
               contentStream
                  << "\"";
               value.value().writeEscaped(
                  contentStream
               );
               contentStream
                  << "\"";
            }
         
         }
   
         contentStream << endl << "}";
   
         _headers["content-type"] =
            "application/json; charset=UTF-8";
            
         _content = contentStream.str();
         _serveFile = false;
   
      }
      
      optional<BString> getKeyFromPath(
         const BString& path
      )
      {
         vector<BString> parts =
            path.split('?');
            
         if (parts.size() == 2)
         {
            vector<BString> queries =
               parts[1].split('&');
            for (const BString query : queries)
            {
               vector<BString> keyValue =
                  query.split('=');
               if ( keyValue.size() == 2 &&
                    keyValue[0] == "key" )
               {
                  return keyValue[1];
               }
            }
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
