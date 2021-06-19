#ifndef BEE_FISH_HTTPS__AUTHENTICATION_H
#define BEE_FISH_HTTPS__AUTHENTICATION_H
#include <exception>
#include <optional>
#include "../database/database.h"
#include "../database/path.h"

using namespace bee::fish::database;
using namespace bee::fish::power_encoding;
using namespace bee::fish::https;

namespace bee::fish::https {

   class Session;
   
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
      
      
   
      inline static const size_t
         SESSION_ID_SIZE = 64;
  
   public:
      // Implemented in session.h
      Authentication(Session* session);
      
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
         _authenticated = false;
         
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
      virtual void logon(const BString& secret)
      {

         if (!_ipAddress.size())
            throw runtime_error("Missing ip-address");
            
         if (!secret.size())
            throw runtime_error("Missing secret");
            
         _authenticated = false;
         
         // Save the secret
         // and set the user data path
         BString md5Secret =
            Data(secret).md5();
         
         _userData = _path
            ["Secrets"]
            [md5Secret];
                  
         // Create the session id
         _sessionId =
            Data::fromRandom(
               SESSION_ID_SIZE
            ).toBase64();
         
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
            
         _authenticated = true;
         
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
         
         _sessionId.clear();
         _authenticated = false;
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