#ifndef BEE_FISH_SERVER__BASIC_AUTHORIZATION_H
#define BEE_FISH_SERVER__BASIC_AUTHORIZATION_H

#include "../parser/parser.h"
#include "base64.h"
#include "request.h"

using namespace bee::fish::parser;

namespace bee::fish::server {

   class BasicAuthorization : public And
   {
   public:
      BString _username;
      BString _password;
      BString _base64;
         
   public:
      BasicAuthorization(
         const string& value
      ) : And(
         new CIWord("Basic"),
         Blanks.copy(),
         new Capture(
            Base64,
            _base64
         ),
         new Character(BString::EndOfFile)
      )
      {
         if (read(value))
         {
         
            string creds =
               base64::decode(_base64);
               
            Credentials credentials(
               creds
            );
            
            _base64.clear();
    
            if (credentials.result())
            {
               _username = credentials._username;
               _password = credentials._password;
               credentials._username.clear();
               credentials._password.clear();
               success();
            }
            else
            {
               fail();
            }
         }
      }
      
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      )
      {
         out << "BasicAuthorization";
         writeResult(out);
         out << "()";
      }
      
      
   private:
      class Credentials : public And
      {
      public:
         BString _username;
         BString _password;
      public:
         Credentials(const string& value) : 
            And(
               new Capture(
                  new Repeat(
                     not Character(':')
                  ),
                  _username
               ),
               new Character(':'),
               new Capture(
                  new Repeat(
                     not Character(
                        BString::EndOfFile
                     )
                  ),
                  _password
               ),
               new Character(BString::EndOfFile)
            )
         {
            read(value);
         }
         

      };
      

   };

}

#endif