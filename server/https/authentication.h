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
      BString _sessionId;
      
      bool _authenticated = false;
      Path<PowerEncoding> _path;
      Path<PowerEncoding> _userData;
      
      inline static const size_t
         SESSION_ID_SIZE = 64;
   
   
      Authentication(Database& database) :
         _database(database),
         _path(database),
         _userData(_path)
      {
      }
      
   public:
      Authentication(
         Session& session,
         BString sessionId = ""
      ) :
         Authentication(
           *( session.server()->database() )
         )
      {
      
         Request& request =
            *( session.request() );
         
         _ipAddress = session.ipAddress();
         
         _sessionId =
            request.getCookie("sessionId");
  
   
         if ( _ipAddress.size() &&
              _sessionId.size() )
         {
            
            Path sessionPath = _path
               ["IP Addresses"]
               [_ipAddress]
               ["Sessions"]
               [_sessionId]
               ["User Data Path"];
               
            if (sessionPath.hasData())
            {
               _authenticated = true;
               sessionPath.getData(
                  _userData._index
               );
            }
         }
         
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
               
               if ( method == "getStatus" )
               {
               /*
                  if (_authenticated)
                     getThumbnail();
                  else
               */
                     _thumbnail.clear();
               }
               else if ( method == "logon" )
               {
                  logon();
               }
               else if ( method == "logoff" )
               {
                  logoff();
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
         // and set the user data path
         _userData = _path
            ["Users"]
            [_name]
            ["Secrets"]
            [_secret];
                  
         // Create the session id
         _sessionId =
            Data::fromRandom(
               SESSION_ID_SIZE
            ).toHex();
         
         // Save the secret path under
         // ip address/session id
         _path
            ["IP Addresses"]
            [_ipAddress]
            ["Sessions"]
            [_sessionId]
            ["User Data Path"]
            .setData(
               _userData._index
            );
            
         // Check for thumbnail
         if (_thumbnail.size())
            setThumbnail();
            
         _thumbnail.clear();
         
         Path thumbnails =
            _userData
            ["Thumbnail"];
               
         if (thumbnails.hasData())
         {
            thumbnails.getData(_thumbnail);
         }
         
         
      }
      
      virtual void logoff()
      {
         if (_sessionId.size())
         {
            Path sessionPath = _path
               ["IP Addresses"]
               [_ipAddress]
               ["Sessions"]
               [_sessionId]
               ["User Data Path"];
               
            sessionPath.clear();
         }
         
         _thumbnail.clear();
         _sessionId.clear();
         _authenticated = false;
      }
      
      virtual void setThumbnail()
      {
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
            
         Path thumbnails =
            _userData
            ["Thumbnail"];
               
         thumbnails.setData(_thumbnail);
         
         if (thumbnails.hasData())
            thumbnails.getData(_thumbnail);
      }
      
      virtual void getThumbnail()
      {
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
         
         // Get the thumbnail
         Path thumbnails =
            _userData
            ["Thumbnail"];
        
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
            
         return _userData;
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
         out <<  endl
             << "\t\"authenticated\": "
                << (_authenticated ?
                   "true" :
                   "false");
                   
         if (_name.size())
         {
            out
               << "," << endl
               << "\t\"name\": \"";
             
            _name.writeEscaped(out);
          
            out << "\"";
         }
         
         if (_thumbnail.size())
            out << "," << endl
                << "\t\"serverThumbnail\": \""
                   << _thumbnail
                << "\"";
        
         out << endl;
           
      }
      
      operator bool()
      {
         return _authenticated;
      }
   };
   
   

};

#endif