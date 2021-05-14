#ifndef BEE_FISH_HTTPS__AUTHENTICATION_H
#define BEE_FISH_HTTPS__AUTHENTICATION_H
#include <exception>
#include <optional>
#include "../database/database.h"
#include "../database/path.h"
#include "session.h"

using namespace bee::fish::database;
using namespace bee::fish::power_encoding;
using namespace bee::fish::https;

namespace bee::fish::https {

   class Authentication
   {
   private:
      Database& _database;
      
      bool _authenticated = false;
      Path<PowerEncoding> _path;
      Path<PowerEncoding> _userData;
      
   protected:
      BString _ipAddress;
      BString _sessionId;
      
   public:
      BString _name;
      BString _secret;
      BString _thumbnail;
      
   
      inline static const size_t
         SESSION_ID_SIZE = 64;
  
   public:
      Authentication(Session* session) :
         Authentication(
            *( session->server()->database() ),
            session->ipAddress(),
            session->
               request()->
               getCookie("sessionId")
         )
      {
  
      }
      
      Authentication(
         Database& database,
         BString ipAddress,
         BString sessionId
      ) :
         _database(database),
         _path(_database),
         _userData(_path),
         _ipAddress(ipAddress),
         _sessionId(sessionId)
      {

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
         


      }
     
   public:
      virtual void logon()
      {
         cerr << "Authentication::logon()" << endl;
   
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
      cerr << "Authentication::setThumbnail()" << endl;
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
            
         if (!_thumbnail.size())
            throw runtime_error("Missing thumbnail");
            
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
         out 
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
                << "\t\"thumbnail\": \""
                   << _thumbnail
                << "\"";
           
      }
      
      operator bool()
      {
         return _authenticated;
      }
      
      bool authenticated()
      {
         return _authenticated;
      }
      
      const BString& sessionId() const
      {
         return _sessionId;
      }
   };
   
   

};

#endif