#ifndef BEE_FISH_SERVER__STORAGE_APP_H
#define BEE_FISH_SERVER__STORAGE_APP_H

#include "../misc/optional.h"
#include "session.h"
#include "app.h"
#include "authentication.h"
#include "storage.h"
#include "../json/json-parser.h"
#include "../b-script/b-script.h"
#include "../web-request/web-request.h"

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
         WebRequest postRequest;
         bool returnValue = false;
         bool returnJSON = true;

         BeeFishMisc::optional<BString> contentType = BeeFishMisc::nullopt;

         bool idInQuery = false;

         // Extract id from the query
         //std::cout << "URL Path:  " << request->path() << std::endl;
         //std::cout << "URL Query: " << request->query() << std::endl;
         BeeFishWeb::WebRequest::URL::Query& query = request->queryObject();

         if (query.count("id") > 0) {
            idInQuery = true;
            id = query["id"];
            if (request->method() == "GET") {
               method = "getItem";
               returnJSON = false;
            }
         }
         else if (request->query().length()) {
            key = request->query();
            if (request->method() == "GET") {
               method = "getItem";
               returnJSON = false;
            }
         }

         std::cout << "ID: ";

         if (id.hasValue()) {
            std::cout << id.value();
         }

         std::cout << std::endl;

         if (request->method() == "POST" && request->hasJSON()) {

            WebRequest postRequest;            

            BeeFishBScript::BScriptParser parser(postRequest);

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

         path = request->path();

         Storage storage(*this, path);


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

         if ( request->method() == "POST" &&
              idInQuery &&
              _id.hasValue() ) 
         {
            std::cout << "Reading the entire response" << std::endl;

            method = "setItem";

            if (request->headers().contains("content-type"))
               contentType = request->headers()["content-type"];

            BeeFishParser::Parser parser(postRequest);

            if (!parseWebRequest(parser))
               throw std::runtime_error("Invalid input post with id to storage-app.h");

            std::cout << "SUCCESSFULLY PARSED WEB REQUEST WITH IMAGE IN BODY" << std::endl;
            
         }

         // Get item with key
         if ( method == BString("getItem") &&
              key != BeeFishMisc::nullopt )
         {
            returnValue = true;
            
            BeeFishMisc::optional<Data> data;
            data =
               storage.getItem(key.value(), contentType);

            if (data.hasValue())
               value = (BString)data.value();
            else
               value = BeeFishMisc::nullopt;

            _status = 200;
         }
         // Get item with id
         else if ( method == BString("getItem") && _id.hasValue() )
         {
            returnValue = true;
               
            BeeFishMisc::optional<Data> data;
            data =
               storage.getItem(_id.value(), contentType);

            if (data.hasValue())
               value = (BString)data.value();
            else
               value = BeeFishMisc::nullopt;
            
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
                  value.value(),
                  BeeFishMisc::nullopt
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
            returnJSON = true;
            returnValue = false;

            if ( value == BeeFishMisc::nullopt )
            {
               storage.removeItem(
                  _id.value()
               );
               _status = 200;
            }
            else if (request->method() == "POST" && idInQuery) {
               Data data(postRequest.body().data(), postRequest.body().length());

               storage.setItem(
                  _id.value(),
                  data,
                  contentType
               );

               std::cout << "SET STATUS To 200 " << std::endl;
               _status = 200;
            }
            else if (_id.hasValue() && value.hasValue())
            {
               storage.setItem(
                  _id.value(),
                  value.value(),
                  BeeFishMisc::nullopt
               );

               _status = 200;
            }
                     
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
            if (contentType.hasValue()) {
               _responseHeaders.replace(
                  "content-type",
                  contentType.value()
               );
            }
            else {
               _responseHeaders.replace(
                  "content-type",
                  "text/plain"
               );
            }

            if ( value != BeeFishMisc::nullopt )
               _content = std::string((const char*)value.value(), value.value().size());
            else
               _content = "";
               
            _serveFile = false;
            
            return;
         }
         else {
            _responseHeaders.replace(
               "content-type",
               "application/json; charset=UTF-8"
            );
            
         }

         BeeFishBScript::Object output;
         
         if ( key != BeeFishMisc::nullopt )
         {
            output["key"] = key.value();
         }
         
         if ( _id != BeeFishMisc::nullopt )
         {
            output["id"] = _id.value().key();
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
      
      virtual BString name()
      {
         return "Storage app";
      }

   };

   

}

#endif
