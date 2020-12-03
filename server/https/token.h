#ifndef BEE_FISH_SERVER__TOKEN_H
#define BEE_FISH_SERVER__TOKEN_H
#include <exception>
#include <database.h>
#include <path.h>
#include <optional>
#include "request.h"
#include "base64.h"
#include "md5.h"
#include "server.h"

using namespace bee::fish::database;

namespace bee::fish::server {

   class Token
   {
   protected:
      const Server* _server;
      string _ipAddress;
      wstring _username;
      string _hash;
      bool _authenticated;
   public:
      Token(
         const Server* server,
         const string& ipAddress,
         const string& hash,
         const wstring& username 
      ) :
         _server(server)
      {
         _authenticated = false;
         _server = server;
         _ipAddress = ipAddress;
         _hash = hash;
         _username = username;
         authenticate(_username, _hash, false);
      }
     
      Token( const Server* server,
             const string& ipAddress,
             const wstring& username,
             const wstring& password )
       
      {
         _authenticated = false;
         _server = server;
         _ipAddress = ipAddress;
         _username = username;
         _hash = md5(
            _username + L":" +
            password  + L"@" +
            _server->hostName()
         );
         authenticate(
            _username,
            _hash,
            true
         );
      }
      
      virtual ~Token()
      {
      }
      
   private:
      virtual void authenticate(
         const wstring& username,
         const string& hash,
         bool confirm
      )
      {
         bee::fish::database::
            Path bookmark(
               *(_server->database())
            );
         
         wcerr << L"Authenticating ";
         String::write(wcerr, username);
         cerr << "...";

         bookmark = bookmark
            ["Users"]
            [username];
 
         if ( bookmark.isDeadEnd()  )
         {
            if (confirm)
            {
               // Need to confirm username/password
               wcerr << L"needs confirmation.";

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
         
            wcerr << L"validating username...";
            
            if (bookmark["Logins"].contains(hash))
            {
               _authenticated = true;
            }
         }
         
         if (_authenticated)
            wcerr << L"authenticated.";
         else
            wcerr << L"Invalid credentials.";
         
         wcerr << endl;
      }
      
   public:
   
      friend ostream&
      operator << (
         ostream& out,
         const Token& token
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
             << "\t\"username\": \"";
          String::write(out, _username);
          out << "\"" << endl
              << "}" << endl;
      }
      
      virtual const string&
      ipAddress() const
      {
         return _ipAddress;
      }
      
      virtual const wstring& username() const
      {
         return _username;
      }
   
      virtual const string& hash() const
      {
         return _hash;
      }
      
      
      bool authenticated() {
         return _authenticated;
      }

   };

};

#endif