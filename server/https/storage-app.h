#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../misc/optional.h"

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

         bee::fish::misc::optional<BString> method = bee::fish::misc::nullopt;
         bee::fish::misc::optional<BString> key = bee::fish::misc::nullopt;
         bee::fish::misc::optional<BString> value = bee::fish::misc::nullopt;
         bee::fish::misc::optional<Id> id = bee::fish::misc::nullopt;
         
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
            try {
               id = Id::fromKey(key);
            }
            catch (...) {
               id = bee::fish::misc::nullopt;
            }
         }
         else if ( request.method() == "GET" )
         {
            key = getKeyFromPath(
                  request
               );

            if (key != bee::fish::misc::nullopt)
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
               value = bee::fish::misc::nullopt;
            else
               value = json->value();
               
         }
         
         
         // Get item with key
         if ( method == "getItem" &&
              key != bee::fish::misc::nullopt )
         {
            returnValue = true;
               
            if (storage.has(key.value()))
            {
               value =
                  storage.getItem(key.value());
            }
            else
               value = bee::fish::misc::nullopt;
               
            _status = "200";
         }
         // Get item with id
         else if ( method == "getItem" &&
              id != bee::fish::misc::nullopt )
         {
            returnValue = true;
               
            if (storage.has(id.value()))
            {
               value =
                  storage.getItem(id.value());
            }
            else
               value = bee::fish::misc::nullopt;
               
            _status = "200";
         }
         // Set item with key
         else if ( method == "setItem" &&
                   key != bee::fish::misc::nullopt )
         {
            if ( value == bee::fish::misc::nullopt )
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
                   id != bee::fish::misc::nullopt )
         {
            if ( value == bee::fish::misc::nullopt )
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
                   key != bee::fish::misc::nullopt )
         {
            storage.removeItem(key.value());
            _status = "200";
         }
         // Remove item with id
         else if ( method == "removeItem" &&
                   id != bee::fish::misc::nullopt )
         {
            storage.removeItem(id.value());
            _status = "200";
         }
         // Clear
         else if (method == "clear")
         {
            key = bee::fish::misc::nullopt;
                     
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
            
            if ( value != bee::fish::misc::nullopt )
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
         
         if ( key != bee::fish::misc::nullopt )
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
         
         if ( id != bee::fish::misc::nullopt )
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
            
            if (value == bee::fish::misc::nullopt)
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
      
      bee::fish::misc::optional<BString> getKeyFromPath(
         const Request& request
      )
      {
         if ( request.path() !=
              "/client/storage/" )
         {
            return bee::fish::misc::nullopt;
         }
            
         const BString& query =
            request.query();
         
         if ( query.size() )
         {
            return query.substr(1);
         }
         
         return bee::fish::misc::nullopt;
      }
   
      virtual BString name()
      {
         return "Storage app";
      }

   };

   

}

#endif
