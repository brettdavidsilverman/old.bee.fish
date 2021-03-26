#ifndef BEE_FISH_HTTPS__AUTHENTICATION_H
#define BEE_FISH_HTTPS__AUTHENTICATION_H
#include <exception>
#include <optional>
#include "../database/database.h"
#include "../database/path.h"
#include "request.h"
#include "base64.h"
#include "server.h"
#include "credentials.h"

using namespace bee::fish::database;
using namespace bee::fish::power_encoding;

namespace bee::fish::https {

   class Authentication
   {
   private:
      BString _hash;
   public:
      Server& _server;
      bee::fish::database::Database& _database;
      bee::fish::database::
         Path<PowerEncoding> _userData;
      BString _ipAddress;
      BString _username;
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
             const Credentials& credentials) :
         Authentication(server)
      {
         _authenticated = false;
         _ipAddress = ipAddress;
         _username = credentials._username;
         _hash = credentials._hash;
         
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
      

   };

};

#endif