#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../misc/optional.h"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"
#include "../json/json-parser.h"

using namespace std;
using namespace BeeFishWeb;

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

         WebRequest* request = _session->request();

         BeeFishBString::BString path;
         BeeFishMisc::optional<BString> method = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> key = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> value = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> id = BeeFishMisc::nullopt;


         if (request->method() == "POST" && request->hasJSON()) {

            request = new WebRequest();            

            JSONParser parser(*request);

            parser.captureValue("method", method);
            parser.captureValue("key", key);
            parser.captureValue("id", id);
            parser.captureValue("value", value);

            
            if (!parseWebRequest(parser))
            {
               delete request;
               throw std::runtime_error("Invalid input to storage-app.h");
            }

            path = request->path();

            delete request;

         }

         Storage storage(*this, path);

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
            if ( value.hasValue() )
            {
               storage.setItem(
                  key.value(),
                  value.value()
               );
            }
            else
            {
               storage.removeItem(
                  key.value()
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
            
         BeeFishJSONOutput::Object output;
         
         if ( key != BeeFishMisc::nullopt )
         {
            output["key"] = key.value();
         }
         else
         {
            output["key"] = nullptr;
         }
         
         if ( _id != BeeFishMisc::nullopt )
         {
            output["id"] = _id.value().key();
         }
         else
         {
            output["id"] = nullptr;
         }
               
         output["response"] = "ok";
                  
         if (returnValue)
         {
            if (value == BeeFishMisc::nullopt)
               output["value"] = nullptr;
            else
               output["value"] = value.value();
         
         }
   
         _content = output.str();
         _serveFile = false;

      }
      
      BeeFishMisc::optional<BString> getKeyFromPath(
         const WebRequest& request
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
