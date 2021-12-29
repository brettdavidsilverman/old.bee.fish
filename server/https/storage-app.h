#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../misc/optional.h"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"
#include "../json/json-parser.h"

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

         Request* request = _session->request();

         BeeFishMisc::optional<BString> method = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> key = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> value = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> id = BeeFishMisc::nullopt;


         if (request->method() == "POST" && request->hasJSON()) {
            request = new Request();
            JSONParser parser(*request);

            parser.captureValue("method", method);
            parser.captureValue("key", key);
            parser.captureValue("id", id);
            parser.captureValue("value", value);

            
            if (!parseRequest(parser))
            {
               throw std::runtime_error("Invalid input to storage-app.h");
            }

         }

         Storage storage(*this, "test");//request.path());

         bool returnValue = false;
         bool returnJSON = true;

         BeeFishMisc::optional<Id> _id;

         // We accept either Id or Key, but not both.
         // Test for Id first
         if (id.hasValue()) {
            BString key = id;
            try {
               _id = Id::fromKey(key);
            }
            catch (...) {
               _id = BeeFishMisc::nullopt;
            }
         }
         else if ( request->method() == "GET" )
         {
            key = getKeyFromPath(
                  *request
               );

            if (key != BeeFishMisc::nullopt)
            {
               method = "getItem";
               returnJSON = false;
            }
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
              _id != BeeFishMisc::nullopt )
         {
            returnValue = true;
               
            if (storage.has(_id.value()))
            {
               value =
                  storage.getItem(_id.value());
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
                   _id != BeeFishMisc::nullopt )
         {
            if ( value == BeeFishMisc::nullopt )
            {
               storage.removeItem(
                  _id.value()
               );
            }
            else
            {
               storage.setItem(
                  _id.value(),
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
                   _id != BeeFishMisc::nullopt )
         {
            storage.removeItem(_id.value());
            _status = "200";
         }
         // Clear
         else if (method == BString("clear"))
         {
            key = BeeFishMisc::nullopt;
                     
            storage.clear();
            _status = "200";
         }
   

         if ( _status != "200" )
            return;
            
         if ( !returnJSON )
         {
            _responseHeaders.replace(
               "content-type",
               "text/plain; charset=UTF-8"
            );
            
            if ( value != BeeFishMisc::nullopt )
               _content = (const char*)value.value();
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
                     
            key.value().writeEscaped(contentStream);
        
            contentStream
               << "\"";
         }
         else
         {
            contentStream
               << "   \"key\": null";
         }
         
         contentStream << "," << endl;
         
         if ( _id != BeeFishMisc::nullopt )
         {
            contentStream
               << "   \"id\":\""
               << _id.value().key()
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
         

         _content = contentStream.str();
         _serveFile = false;

      }
      
      BeeFishMisc::optional<BString> getKeyFromPath(
         const Request& request
      )
      {
         if ( request.path() !=
              BString("/client/storage/") )
         {
            return BeeFishMisc::nullopt;
         }
            
         const BString& query =
            request.query();
         
         if ( query.size() )
         {
            return query;
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
