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
      bee::fish::database::Path _bookmark;
   public:
      Token(
         const Server* server,
         const string& ipAddress,
         const string& hash,
         const wstring& username 
      ) :
         _server(server),
         _bookmark(*(server->database()))
      {
         _authenticated = false;
         _server = server;
         _ipAddress = ipAddress;
         _hash = hash;
         _username = username;
         authenticate(_hash, _username, false);
      }
     
      Token( const Server* server,
             const string& ipAddress,
             const wstring& username,
             const wstring& password )
         : _bookmark(*(server->database()))
       
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
         authenticate(_hash, _username, true);
      }
      
      virtual ~Token()
      {
      }
      
   private:
      virtual void authenticate(
         const string& hash,
         const wstring& username,
         bool confirm
      )
      {
         wcerr << L"Authenticating ";
         String::write(wcerr, username);
         cerr << "...";

         _bookmark = Branch::Root;
         
         _bookmark << L"credentials";
         
         _bookmark << hash;
 
         if ( _bookmark.isDeadEnd()  )
         {
            if (confirm)
            {
               // Need to confirm username/password
               wcerr << L"needs confirmation.";

               // Write out the username, to be
               // authenticated on next request
               _bookmark << L"username"
                         << username;

            }
            
            _authenticated = false;
         }
         else
         {
         
            wcerr << L"validating username...";
            
            try {
               // Confirm username address
               ReadOnlyPath pointer(_bookmark);
               pointer << L"username" << username;
               _authenticated = true;
               wcerr << L"authenticated.";
            }
            catch(...) {
               _authenticated = false;
               wcerr << L"Invalid credentials.";
            }
            
         }
         
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

}

#endif