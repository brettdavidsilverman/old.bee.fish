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
         Headers& responseHeaders
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
            _JSON* json = object["value"];
            if ( json->isNull() )
               value = nullopt;
            else
               value = json->value();
               
         }
         
         // Execute the method
         optional<Id> id;
         
         if (key != nullopt)
         {
            try
            {
               id = Id::fromKey(key.value());
            }
            catch (exception& ex)
            {
               _status = "500";
               responseHeaders.replace(
                  "content-type",
                  "application/json; charset=UTF-8"
               );
               
               BString error = ex.what();
               
               stringstream stream;
               stream << "\"";
               stream << "Invalid Id key. ";
               error.writeEscaped(stream);
               stream << "\"";
               
               _content = stream.str();
               
               cerr << _content << endl;
               return;
            }
         }
         
         // Get item
         if ( method == "getItem" &&
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
         // Set item
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
         // Remove item
         else if ( method == "removeItem" &&
                   id != nullopt )
         {
            storage.removeItem(id.value());
            _status = "200";
         }
         // Clear
         else if (method == "clear")
         {
            id = nullopt;
                     
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
            
         std::ostringstream contentStream;
   
         contentStream << "{" << endl;
         
         if ( id != nullopt )
         {
            contentStream
               << "   \"key\":\"";
                     
            id.value().key()
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
               contentStream << "\"";
               value.value().writeEscaped(contentStream);
               contentStream << "\"";
            }
         
         }
   
         contentStream << endl << "}";
   
         _content = contentStream.str();
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
