#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../misc/optional.h"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"
#include "../json/json-parser.h"
#include "../b-script/b-script.h"

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
         BeeFishBScript::ObjectPointer json;
         BeeFishMisc::optional<BString> method = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> key = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> value = BeeFishMisc::nullopt;
         BeeFishMisc::optional<BString> id = BeeFishMisc::nullopt;


         if (request->method() == "POST" && request->hasJSON()) {

            WebRequest postRequest;            

            BeeFishBScript::BScriptParser parser(postRequest);

            /*
            //JSONParser parser(postRequest);

            parser.captureValue("method", method);
            parser.captureValue("key", key);
            parser.captureValue("id", id);
            parser.captureValue("value", value);
            */
            
            if (!parseWebRequest(parser))
            {
               throw std::runtime_error("Invalid input to storage-app.h");
            }

            json = parser.json();
            
            if (json->contains("method"))
               method = (*json)["method"];

            if (json->contains("key"))
               key = (*json)["key"];

            if (json->contains("id"))
               id = (*json)["id"];

            if (json->contains("value"))
               value = (*json)["value"];

         }
         else if (request->method() == "GET") {
            
            BeeFishBString::BString query = request->query();

            std::vector<BString> keyValues = query.split('&');
            for (const BString& pair : keyValues) {
               if (pair.startsWith("id=")) {
                  id = pair.substr(3);
                  break;
               }
            }

         }

         path = request->path();

         Storage storage(*this, path);

         bool returnValue = false;
         bool returnJSON = true;

         BeeFishMisc::optional<Id> _id;

         // We accept either Id or Key, but not both.
         // Test for Id first
         if (id.hasValue()) {
            try {
               _id = Id::fromKey(id.value());
            }
            catch (...) {
               _id = BeeFishMisc::nullopt;
            }
         }
         
         if ( request->method() == "GET" )
         {
            key = getKeyFromPath(
                  *request
               );

            if (key != BeeFishMisc::nullopt || _id != BeeFishMisc::nullopt)
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
               
            value =
               storage.getItem(key.value());

            _status = 200;
         }
         // Get item with id
         else if ( method == BString("getItem") && _id.hasValue() )
         {
            returnValue = true;
               
            value =
               storage.getItem(_id.value());
            
            _status = 200;
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

            _status = 200;
               
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
                     
            _status = 200;
               
         }
         // Remove item with key
         else if ( method == BString("removeItem") &&
                   key != BeeFishMisc::nullopt )
         {
            storage.removeItem(key.value());
            _status = 200;
         }
         // Remove item with id
         else if ( method == BString("removeItem") &&
                   _id != BeeFishMisc::nullopt )
         {
            storage.removeItem(_id.value());
            _status = 200;
         }
         // Clear
         else if (method == BString("clear"))
         {
            key = BeeFishMisc::nullopt;
                     
            storage.clear();
            _status = 200;
         }
   


         if ( _status != 200 )
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
            
         BeeFishBScript::Object output;
         
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
