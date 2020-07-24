#ifndef BEE_FISH_SERVER__TOKEN_H
#define BEE_FISH_SERVER__TOKEN_H
#include <exception>
#include <database.h>
#include "request.h"
#include "base64.h"
#include "md5.h"
#include "server.h"
namespace bee::fish::server {

   class Token
   {
   protected:
      const server* _server;
      string _ip_address;
      string _email;
      string _hash;
      bool _authenticated;
      Pointer _bookmark;
   public:
      Token()
      {
         _authenticated = false;
      }
     
      Token( const server* s,
             const string& ip_address,
             const string& email,
             const string& password )
      {
         _server = s;
         _ip_address = ip_address;
         _email = email;
         _hash = md5(
            _server->get_host_name() + ":" +
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
         Database* database =
            _server->get_database();
         const Database* read_only;
         database->pointer = 0;
         database->walkPath(_hash);
         Pointer bookmark =
            database->pointer;
         Pointer* data = database->_array;
         
         if ( data[bookmark    ] == 0 &&
              data[bookmark + 1] == 0 )
         {
            // Need to confirm username/password
            _authenticated = false;
            // Write out the email, to be
            // authenticated on next request
            database->walkPath(_email);
         }
         else {
            _authenticated = true;
            /*
            try {
               // Confirm email address
               if (read_only->walkPath(_email, bookmark) != 0) {
                  _authenticated = true;
               }
               else
                  _authenticated = false;
            }
            catch(exception& ex) {
               cerr << ex.what();
            }
            */
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
             << _server->get_host_name()
             << ","
             << _ip_address
             << ","
             << _email
             << ")" << endl;
      }
      
      virtual const string&
      ip_address() const
      {
         return _ip_address;
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