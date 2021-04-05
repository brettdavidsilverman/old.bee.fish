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
      BString _username;
      BString _secret;
      bool _authenticated;
      Path<PowerEncoding> _userData;
      
   
   
      Authentication(Database& database) :
         _database(database),
         _userData(database)
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
      
            if ( body.contains("username") )
               _username = body["username"]->value();
                 
            if ( body.contains("secret") )
               _secret = body["secret"]->value();
  
            if ( body.contains("method") &&
                 body["method"]->value()
                    == "logon" )
            {
               logon();
            }
            

         }
         

      }
     
   public:
      virtual void logon()
      {
         if (!_ipAddress.size())
            throw runtime_error("Missing ip-address.");
            
         if (!_username.size())
            throw runtime_error("Missing username.");
            
         if (!_secret.size())
            throw runtime_error("Missing secret.");
            
         Path bookmark(_database);
         
         bool userExists =
            bookmark
               ["Users"]
               .contains(_username);
               
         if (userExists)
         {
            _authenticated =
               bookmark
                  ["Users"]
                  [_username]
                  ["Secrets"]
                  .contains(_secret);
         }
         else
         {
            // new user
            _authenticated = true;
            // Save the secret
            bookmark
               ["Users"]
               [_username]
               ["Secrets"]
                  << _secret;
         }
         
         if (_authenticated)
         {
            _userData = 
               bookmark
                  ["Users"]
                  [_username]
                  ["Data"];
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
             << "\t\"username\": \"";
             
         _username.writeEscaped(out);
          
         out << "\"" << endl
             << "}" << endl;
      }
      
      operator bool()
      {
         return _authenticated;
      }
   };
   
   

};

#endif