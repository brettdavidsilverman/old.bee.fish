#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../misc/optional.h"

#include "../config.h"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"


using namespace std;

namespace BeeFishHTTPS {

   class StorageApp : public App {
   public:
      StorageApp(
         Session* session,
         ResponseHeaders& responseHeaders
      ) : App(session, responseHeaders)
      {
      }


      virtual void handleResponse() {
         
         if (!authenticated())
            return;

         Request request;
         Parser parser(request);
         ifstream input(_session->tempFileName());

         JSON& json =
            *( request._json );

         Object& object =
            *( json._object );
            
         Storage storage(*this, "test");//request.path());

         BeeFishMisc::optional<BString> method = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> key = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> value = BeeFishMisc::nullopt;
         BeeFishMisc::optional<Id> id = BeeFishMisc::nullopt;
         
         bool returnValue = false;
         bool returnJSON = true;
#warning "Storage app needs to be rewritten"

         /*
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
               id = BeeFishMisc::nullopt;
            }
         }
         else if ( request.method() == "GET" )
         {
            key = getKeyFromPath(
                  request
               );

            if (key != BeeFishMisc::nullopt)
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
               value = BeeFishMisc::nullopt;
            else
               value = json->value();
               
         }
         
         
         // Get item with key
         if ( method == BString("getItem") &&
              key != BeeFishMisc::nullopt )
         {
            returnValue = true;
               
            if (storage.has(key.value()))
            {
               value =
                  storage.getItem(key.value());
            }
            else
               value = BeeFishMisc::nullopt;
               
            _status = "200";
         }
         // Get item with id
         else if ( method == BString("getItem") &&
              id != BeeFishMisc::nullopt )
         {
            returnValue = true;
               
            if (storage.has(id.value()))
            {
               value =
                  storage.getItem(id.value());
            }
            else
               value = BeeFishMisc::nullopt;
               
            _status = "200";
         }
         // Set item with key
         else if ( method == BString("setItem") &&
                   key != BeeFishMisc::nullopt )
         {
            if ( value == BeeFishMisc::nullopt )
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
         else if ( method == BString("setItem") &&
                   id != BeeFishMisc::nullopt )
         {
            if ( value == BeeFishMisc::nullopt )
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
         else if ( method == BString("removeItem") &&
                   key != BeeFishMisc::nullopt )
         {
            storage.removeItem(key.value());
            _status = "200";
         }
         // Remove item with id
         else if ( method == BString("removeItem") &&
                   id != BeeFishMisc::nullopt )
         {
            storage.removeItem(id.value());
            _status = "200";
         }
         // Clear
         else if (method == BString("clear"))
         {
            key = BeeFishMisc::nullopt;
                     
            storage.clear();
            _status = "200";
         }
         */

         if ( _status != "200" )
            return;
            
         if ( !returnJSON )
         {
            _responseHeaders.replace(
               "content-type",
               "text/plain; charset=UTF-8"
            );
            
            if ( value != BeeFishMisc::nullopt )
               _content = value.value();
            else
               _content = "";
               
            _serveFile = false;
            
            return;
         }
         else
            _responseHeaders.replace(
               "content-type",
               "application/json; charset=UTF-8"
            );
            
         std::ostringstream contentStream;
   
         contentStream << "{" << endl;
         
         if ( key != BeeFishMisc::nullopt )
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
         
         if ( id != BeeFishMisc::nullopt )
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
            
            if (value == BeeFishMisc::nullopt)
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
      
      BeeFishMisc::optional<BString> getKeyFromPath(
         const Request& request
      )
      {
         if ( request.path() !=
              "/client/storage/" )
         {
            return BeeFishMisc::nullopt;
         }
            
         const BString& query =
            request.query();
         
         if ( query.size() )
         {
            return query.substr(1);
         }
         
         return BeeFishMisc::nullopt;
      }
   
      virtual BString name()
      {
         return "Storage app";
      }

   };

   

}

#endif
