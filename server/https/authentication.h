#ifndef BEE_FISH_HTTPS__AUTHENTICATION_H
#define BEE_FISH_HTTPS__AUTHENTICATION_H
#include <exception>
#include <optional>
#include "../database/database.h"
#include "../database/path.h"
#include "request.h"
#include "server.h"
#include "credentials.h"

using namespace bee::fish::database;
using namespace bee::fish::power_encoding;
using namespace bee::fish::https;

namespace bee::fish::https {

   class Authentication
   {
   protected:
      Database& _database;
   public:
      BString _ipAddress;
      BString _name;
      BString _secret;
      BString _thumbnail;
      
      bool _authenticated = false;
      Path<PowerEncoding> _path;
      
   
   
      Authentication(Database& database) :
         _database(database),
         _path(database)
      {
      }
      
   public:
      Authentication(
         Session& session
      ) :
         Authentication(
           *( session.server()->database() )
         )
      {
      
         Request& request =
            *( session.request() );
         
         _ipAddress = session.ipAddress();
         
         if (request.hasBody())
         {
            _Object& body = request.body();
      
            if ( body.contains("name") )
            {
               _name = body["name"]->value();
            }
            
            if ( body.contains("secret") )
               _secret = body["secret"]->value();
  
            if ( body.contains("thumbnail") )
               _thumbnail = body["thumbnail"]->value();
               
            if ( body.contains("method") )
            {
               const BString& method =
                  body["method"]->value();
                    
               if ( method == "logon" )
               {
                  logon();
               }
               else if ( method == "setThumbnail" )
               {
                  setThumbnail();
               }
               else if ( method == "getThumbnail" )
               {
                  getThumbnail();
               }
               
            }
            

         }
         

      }
     
   public:
      virtual void logon()
      {
         if (!_ipAddress.size())
            throw runtime_error("Missing ip-address");
            
         if (!_name.size())
            throw runtime_error("Missing name");

         if (!_secret.size())
            throw runtime_error("Missing secret");
            
         _authenticated = true;
         
         // Save the secret
         _path
            ["Users"]
            [_name]
            ["Secrets"]
            [_secret];
                  
         // Check for thumbnail
         Path thumbnails = _path
            ["Users"]
            [_name]
            ["Secrets"]
            [_secret]
            ["Thumbnails"];
            
         if (_thumbnail.size())
            setThumbnail();
            
         _thumbnail.clear();
         
         if (thumbnails.hasData())
         {
            thumbnails.getData(_thumbnail);
         }
         
         
      }
      
      virtual void setThumbnail()
      {
         if (!_name.size())
            throw runtime_error("Missing name");
            
         if (!_secret.size())
            throw runtime_error("Missing secret");

         if (!_thumbnail.size())
            throw runtime_error("Missing thumbnail");
            
         Path thumbnails = _path
               ["Users"]
               [_name]
               ["Secrets"]
               [_secret]
               ["Thumbnails"];
               
         thumbnails.setData(_thumbnail);
         
      }
      
      virtual void getThumbnail()
      {
         // Get the thumbnail
         Path thumbnails = _path
               ["Users"]
               [_name]
               ["Secrets"]
               [_secret]
               ["Thumbnails"];
               
         _thumbnail.clear();
         
         if (thumbnails.hasData())
            thumbnails.getData(_thumbnail);
         
      }
      
   public:
   
      bee::fish::database::
         Path<PowerEncoding> userData()
      {
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
            
         return
            _path
               ["Users"]
               [_name]
               ["Secrets"]
               [_secret]
               ["Data"];
      }
      
      friend ostream&
      operator << (
         ostream& out,
         const Authentication& token
      )
      {
         token.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out) const
      {
         out << "{" << endl
             << "\t\"authenticated\": "
                << (_authenticated ?
                   "true" :
                   "false")
             << "," << endl
             << "\t\"name\": \"";
             
         _name.writeEscaped(out);
          
         out << "\"";
         
         if (_thumbnail.size())
            out << "," << endl
                << "\t\"thumbnail\": \""
                   << _thumbnail
                << "\"";
        
         out << endl
             << "}";
           
      }
      
      operator bool()
      {
         return _authenticated;
      }
   };
   
   

};

#endif