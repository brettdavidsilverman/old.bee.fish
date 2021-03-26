#ifndef BEE_FISH_SERVER__BASIC_AUTHORIZATION_H
#define BEE_FISH_SERVER__BASIC_AUTHORIZATION_H

#include "../parser/parser.h"
#include "../b-string/data.h"

#include "request.h"

using namespace bee::fish::parser;

namespace bee::fish::https {

   class BasicAuthorization
   {
   public:
      BString _username;
      BString _password;
      BString _base64;
      bool    _result = false;
   public:
      BasicAuthorization(
         const BString& value
      )
      {
         And match = And(
            new CIWord("Basic"),
            new bee::fish::https::Request::Blanks(),
            new Capture(
               new bee::fish::parser::Base64(),
               _base64
            )
         );
         
         optional<bool> read =
            match.read(value);
            
         if (read == true)
         {
            
            Data data =
               Data::fromBase64(_base64);
            
            BString decoded = data;

            Credentials credentials(
               decoded
            );
            
            _base64.clear();
    
            if (credentials._result == true)
            {
               _username = credentials._username;
               _password = credentials._password;
               credentials._username.clear();
               credentials._password.clear();
               _result = true;
            }
            else
            {
               _result = false;
            }
         }
      }
      
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      )
      {
         out << "BasicAuthorization<"
             << _result
             << ">()";
            
      }
      
      
   private:
      class Credentials
      {
      public:
         BString _username;
         BString _password;
         bool _result = false;
      public:
         Credentials(const BString& value)
         {
            vector<BString> parts =
               value.split(':');
            
            _result = (parts.size() == 2);
            
            if (_result)
            {
               _username = parts[0];
               _password = parts[1];
            }
 
         }
         

      };
      

   };

}

#endif