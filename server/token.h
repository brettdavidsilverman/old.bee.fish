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
      string _username;
      string _hash;
      string _bookmarkHash;
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
             const string& username,
             const string& password )
         : _bookmark(server->database())
       
      {

         _server = server;
         _ipAddress = ipAddress;
         _username = username;
         _hash = md5(
            _server->hostName() + ":" +
            _username + ":" +
            password
         );
         authenticate(_hash);
      }
      
      virtual ~Token()
      {
      }
      
      virtual void authenticate(
         string hash
      )
      {
         
         _bookmark = 0;
         _bookmark << _hash;
 
         if ( _bookmark.isDeadEnd() )
         {
            // Need to confirm username/password
            _authenticated = false;
            // Write out the username, to be
            // authenticated on next request
            _bookmark << _username;
         }
         else {
            
            try {
               // Confirm username address
               ReadOnlyPointer pointer(_bookmark);
               pointer << _username;
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
         out << "{" << endl
             << "\t\"authenticated\": "
                << (_authenticated ?
                   "true" :
                   "false")
             << "," << endl
             << "\t\"hash\":\""
                << _hash
             << "\"" << endl
             << "}" << endl;
      }
      
      virtual const string&
      ipAddress() const
      {
         return _ipAddress;
      }
      
      virtual const string& username() const
      {
         return _username;
      }
   
      virtual const string& hash() const
      {
         return _hash;
      }
      
      virtual const string& bookmarkHash() const
      {
         return _bookmarkHash;
      }
      
      bool authenticated() {
         return _authenticated;
      }

   };

}

#endif