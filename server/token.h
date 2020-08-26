#ifndef BEE_FISH_SERVER__TOKEN_H
#define BEE_FISH_SERVER__TOKEN_H
#include <exception>
#include <database.h>
#include <optional>
#include "request.h"
#include "base64.h"
#include "md5.h"
#include "server.h"
#include "pointer.h"

namespace bee::fish::server {

   class Token
   {
   protected:
      const Server* _server;
      string _ipAddress;
      string _email;
      string _hash;
      bool _authenticated;
      Pointer _bookmark;
   public:
      Token(Server* server) :
         _server(server),
         _bookmark(server->database())
      {
         _authenticated = false;
      }
     
      Token( const Server* server,
             const string& ipAddress,
             const string& email,
             const string& password )
         : _bookmark(server->database())
       
      {
         cout << "Token::Token("
              << email 
              << ")" 
              << endl;
         
         _server = server;
         _ipAddress = ipAddress;
         _email = email;
         _hash = md5(
            _server->hostName() + ":" +
            _email + ":" +
            password
         );
         authenticate(_hash, _email);
      }
      
      virtual void authenticate(
         string hash,
         string email
      )
      {
         
         _bookmark = 0;
         _bookmark << _hash;
 
         if ( _bookmark.isDeadEnd() )
         {
            // Need to confirm username/password
            _authenticated = false;
            // Write out the email, to be
            // authenticated on next request
            _bookmark << _email;
         }
         else {
            
            try {
               // Confirm email address
               ReadOnlyPointer pointer(_bookmark);
               pointer << _email;
               bool exists = 
                 !pointer.eof();
                  
               if ( exists )
               {
                  _authenticated = true;
               }
               else
                  _authenticated = false;
            }
            catch(exception& ex) {
               cerr << ex.what();
               throw ex;
            }
            
         }
      }
      
      friend ostream&
      operator << (
         ostream& out,
         const Token& token
      )
      {
         token.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out) const {
         out << "Token("
             << _server->hostName()
             << ","
             << _ipAddress
             << ","
             << _email
             << ")" << endl;
      }
      
      virtual const string&
      ipAddress() const
      {
         return _ipAddress;
      }
      
      virtual const string& email() const
      {
         return _email;
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