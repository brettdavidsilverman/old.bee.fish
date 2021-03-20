#ifndef BEE_FISH_SERVER__TOKEN_H
#define BEE_FISH_SERVER__TOKEN_H
#include <exception>
#include <optional>
#include "../database/database.h"
#include "../database/path.h"
#include "request.h"
#include "base64.h"
#include "server.h"

using namespace bee::fish::database;
using namespace bee::fish::power_encoding;

namespace bee::fish::server {

   class Authentication
   {
   protected:
      Server& _server;
      bee::fish::database::Database& _database;
      bee::fish::database::
         Path<PowerEncoding> _userData;
      BString _ipAddress;
      BString _username;
      BString _hash;
      bool _authenticated;
   protected:
      Authentication(Server* server) :
         _server(*server),
         _database(*(_server.database())),
         _userData(_database)
      {
      }

         
   public:
      Authentication( Server* server,
             const BString& ipAddress,
             const BString& username,
             const BString& password ) :
         Authentication(server)
      {
         _authenticated = false;
         _ipAddress = ipAddress;
         _username = username;
         
         BString data =
            _username + ":" +
            password  + "@" +
            _server.hostName();
            
         _hash = data.md5();
         
         authenticate(
            _username,
            _hash,
            true
         );
      }
      
      virtual ~Authentication()
      {
      }
      
   private:
      virtual void authenticate(
         const BString& username,
         const BString& hash,
         bool confirm
      )
      {
         bee::fish::database::
            Path bookmark(_userData);

         bookmark << "Users" << username;
 
         if ( bookmark.isDeadEnd()  )
         {
            if (confirm)
            {
               // Need to confirm username/password
               
               // Write out the login, to be
               // authenticated on next request
               bookmark
                  ["Logins"]
                  << hash;

            }
            
            _authenticated = false;
         }
         else
         {
         
            if (bookmark["Logins"].contains(hash))
            {
               _authenticated = true;
            }
         }
         
         if (_authenticated)
         {
            _userData = bookmark["data"];
         }
         
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
         out << "{" << endl
             << "\t\"authenticated\" : "
                << (_authenticated ?
                   "true" :
                   "false")
             << "," << endl
             << "\t\"hash\": \""
                << _hash
             << "\"," << endl
             << "\t\"username\": \""
             << _username
             << "\"" << endl
             << "}" << endl;
      }
      
      virtual const BString&
      ipAddress() const
      {
         return _ipAddress;
      }
      
      virtual const BString& username() const
      {
         return _username;
      }
   
      virtual const BString& hash() const
      {
         return _hash;
      }
      
      
      bool authenticated() {
         return _authenticated;
      }

   };

};

#endif