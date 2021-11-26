#ifndef BEE_FISH_HTTPS__AUTHENTICATION_H
#define BEE_FISH_HTTPS__AUTHENTICATION_H
#include <exception>
#include "../misc/optional.h"
#include <ctime>
#include "../database/database.h"
#include "../database/path.h"

using namespace BeeFishDatabase;
using namespace BeeFishPowerEncoding;
using namespace BeeFishHTTPS;

namespace BeeFishHTTPS {

   class Session;
   
   class Authentication
   {
   private:
      Database& _database;
      bool _authenticated = false;
      Path<PowerEncoding> _path;
      Path<PowerEncoding> _sessionData;
      Path<PowerEncoding> _userData;
      
   protected:
      BString _ipAddress;
      BString _sessionId;
      
   public:
      
      static time_t epoch_seconds()
      {
         std::time_t result = std::time(nullptr);
         std::localtime(&result);
         return result;
      }
      
   
      inline static const size_t
         SESSION_ID_SIZE = 32;
  
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
         _sessionData(_path),
         _userData(_path),
         _ipAddress(ipAddress),
         _sessionId(sessionId)
      {
         _authenticated = false;
         
         if ( _ipAddress.size() &&
              _sessionId.size() )
         {
            
            _sessionData = _path
               ["IP Addresses"]
               [_ipAddress]
               ["Sessions"]
               [_sessionId];
               
            if ( _sessionData
                    ["Last Authentication"]
                    .hasData() )
            {
          
               time_t lastTime;
               
               _sessionData
                  ["Last Authentication"]
                  .getData(lastTime);
                  
               // 1 hour duration
               if ( (epoch_seconds() - lastTime) 
                    < 60*60 )
               {
                  _authenticated = true;
               
                  _sessionData["User Data Path"]
                     .getData(
                        _userData._index
                     );
                     
                  _sessionData
                     ["Last Authentication"]
                     .setData(epoch_seconds());
               }
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
            ).toHex();

  
         // get the session data
         _sessionData = _path
               ["IP Addresses"]
               [_ipAddress]
               ["Sessions"]
               [_sessionId];
               

         // Save the secret path
         _sessionData["User Data Path"]
            .setData(
               _userData._index
            );

            
         // Save last autgenticated
         time_t lastAuthentication = epoch_seconds();

         _sessionData["Last Authentication"]
            .setData(lastAuthentication);
            

         _authenticated = true;

         
      }
      
      virtual void logoff()
      {
         if (_authenticated)
         {
            _sessionData.clear();
         }
         
         _sessionId.clear();
         _authenticated = false;
      }
      
      
   public:
   
      BeeFishDatabase::
         Path<PowerEncoding> userData()
      {
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
            
         return _userData;
      }
      
      BeeFishDatabase::
         Path<PowerEncoding> sessionData()
      {
         if (!_authenticated)
            throw runtime_error("Unauthenticated");
            
         return _sessionData;
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