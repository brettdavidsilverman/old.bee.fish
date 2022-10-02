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

         BeeFishBString::BString        path;
         BeeFishBScript::ObjectPointer  json;
         BeeFishMisc::optional<BString> key = BeeFishMisc::nullopt;
         BeeFishMisc::optional<Id>      id = BeeFishMisc::nullopt;

         BeeFishMisc::optional<BString> contentType = BeeFishMisc::nullopt;
         Data                           data;
         WebRequest                     postRequest;
         std::string                    string;

         bool returnJSON = true;

         // Extract id from the query
         BeeFishWeb::WebRequest::URL::Query& query = request->queryObject();

         if (query.contains("id")) {

            BString queryID = query["id"];
            queryID = queryID.decodeURI();

            // Test for correct Id
            try {
               id = Id::fromKey(queryID);
            }
            catch (...) {
               id = BeeFishMisc::nullopt;
            }

         }

         if (query.contains("key")) {
            key = query["key"];
         }

         if (!id.hasValue() && !key.hasValue())
            return;

         const BString& method = request->method();

         if (method == "POST") {

            BeeFishBScript::BScriptParser parser(postRequest);

            std::stringstream stream;

            postRequest.setOnData(
               [&stream](const Data& data) {
                  stream.write((const char*)data._data, data.size());
               }
            );

            if (!parseWebRequest(parser)) {
               throw std::runtime_error("Invalid input post with to storage-app.h");
            }

            if (postRequest.headers().contains("content-type"))
               contentType = postRequest.headers()["content-type"];
            else
               contentType = BString("text/plain; charset=utf-8");

            if (postRequest.hasJSON()) {
               string = parser.json().str();
            } else if (postRequest.headers().contains("content-length")) {
               string = stream.str();
            }

            data = Data(string.data(), string.size());

         }

         path = request->path();

         Storage storage(*this, path);

         // Get item with key
         if ( method == "GET" && (id.hasValue() || key.hasValue()))
         {

            if (id.hasValue())  {
               storage.getItem(id.value(), contentType, data);
            }
            else if (key.hasValue()) {
               storage.getItem(key.value(), contentType, data);
            }

            if (contentType.hasValue()) {
               _status = 200;
               returnJSON = false;
            }
            else {
               _status = 404;
               returnJSON = true;
            }

         }
         else if ( method == "POST")
         {
            if (id.hasValue()) {
               storage.setItem(
                  id.value(),
                  contentType,
                  data
               );
            }
            else if (key.hasValue()) {
               storage.setItem(
                  key.value(),
                  contentType,
                  data
               );
            }

            returnJSON = true;
            _status = 200;
                     
         }
         else if (method == "DELETE") {

            if (id.hasValue()) {
               storage.removeItem(id.value());
               _status = 200;
            }
            else if (key.hasValue()) {
               storage.removeItem(key.value());
               _status = 200;
            }
            returnJSON = true;
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

            if ( data.size() ) {
               _data = data;
               _serve = App::SERVE_DATA;
            }
            else {
               _content = "";
               _serve = App::SERVE_CONTENT;
            }
               

            return;
         }
         else {
            _responseHeaders.replace(
               "content-type",
               "application/json; charset=UTF-8"
            );
            
         }

         BeeFishBScript::Object output;
         
         output["method"] = method;
         
         if ( key != BeeFishMisc::nullopt )
         {
            output["key"] = key.value();
         }
         
         if ( id != BeeFishMisc::nullopt )
         {
            output["id"] = id.value().key();
         }
               
         output["response"] = "ok";
                  
         _content = output.str();

         _serve = App::SERVE_CONTENT;

      }
      
      virtual BString name()
      {
         return "Storage app";
      }

   };

   

}

#endif
