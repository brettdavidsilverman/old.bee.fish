#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include <optional>

#include "../config.h"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"


using namespace std;

namespace bee::fish::https {

   class StorageApp : public App {
   public:
      StorageApp(
         Session* session,
         ResponseHeaders& responseHeaders
      ) : App(session, responseHeaders)
      {
   
         Request& request =
            *( session->request() );
         
         if (!authenticated())
            return;

         _JSON& json =
            *( request._json );

         _Object& object =
            *( json._object );
            
         Storage storage(*this, "test");//request.path());

         optional<BString> method = nullopt;
         optional<BString> key = nullopt;
         optional<BString> value = nullopt;
         optional<Id> id = nullopt;
         
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
              object.contains("key") &&
              !(object["key"]->isNull()) )
         {
            key =
               object["key"]->value();
         }
         else if ( request.method() == "POST" &&
              object.contains("id") &&
              !(object["id"]->isNull()) )
         {
            BString key =
               object["id"]->value();
            id = Id::fromKey(key);
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
            _JSON* json = object["value"];
            if ( json->isNull() )
               value = nullopt;
            else
               value = json->value();
               
         }
         
         
         // Get item with key
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
         // Get item with id
         else if ( method == "getItem" &&
              id != nullopt )
         {
            returnValue = true;
               
            if (storage.has(id.value()))
            {
               value =
                  storage.getItem(id.value());
            }
            else
               value = nullopt;
               
            _status = "200";
         }
         // Set item with key
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
         // Set item with id
         else if ( method == "setItem" &&
                   id != nullopt )
         {
            if ( value == nullopt )
            {
               storage.removeItem(
                  id.value()
               );
            }
            else
            {
               storage.setItem(
                  id.value(),
                  value.value()
               );
            }
                     
            _status = "200";
               
         }
         // Remove item with key
         else if ( method == "removeItem" &&
                   key != nullopt )
         {
            storage.removeItem(key.value());
            _status = "200";
         }
         // Remove item with id
         else if ( method == "removeItem" &&
                   id != nullopt )
         {
            storage.removeItem(id.value());
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
            responseHeaders.replace(
               "content-type",
               "text/plain; charset=UTF-8"
            );
            
            if ( value != nullopt )
               _content = value.value();
            else
               _content = "";
               
            _serveFile = false;
            
            return;
         }
         else
            responseHeaders.replace(
               "content-type",
               "application/json; charset=UTF-8"
            );
            
         std::wostringstream contentStream;
   
         contentStream << "{" << endl;
         
         if ( key != nullopt )
         {
            contentStream
               << "   \"key\":\"";
                     
            key
              .value()
              .writeEscaped(contentStream);
        
            contentStream
               << "\"";
         }
         else
         {
            contentStream
               << "   \"key\":null";
         }
         
         contentStream << "," << endl;
         
         if ( id != nullopt )
         {
            contentStream
               << "   \"id\":\""
               << id.value().key()
               << "\"";
         }
         else
         {
            contentStream
               << "   \"id\":null";
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
               contentStream << "\"";
               value.value().writeEscaped(contentStream);
               contentStream << "\"";
            }
         
         }
   
         contentStream << endl << "}";
   
         _content = BString(contentStream.str());
         _serveFile = false;
   
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
