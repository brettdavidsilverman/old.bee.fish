#ifndef BEE_FISH_SERVER__BASIC_AUTHORIZATION_H
#define BEE_FISH_SERVER__BASIC_AUTHORIZATION_H

#include "request.h"
#include "base64.h"
#include "md5.h"

using namespace bee::fish::parser;

namespace bee::fish::server {

   class BasicAuthorization :
      public And {
   protected:
      string _email;
      
   public:
      BasicAuthorization(
         const Header& header
      ) : And(
         new CIWord("Basic"),
         new Blanks(),
         new Base64(),
         new Character(Match::eof)
      )
      {
         read(header.value());
         if (success() == true) {
         
            string credentials =
               base64::decode(base64());
               
            And parser =
            (
               Repeat(not Character('@')) and
               Character('@') and
               Repeat(not (
                  Character(':') or
                  Character('@') )
               ) 
            ) and
            Character(':') and
            (
               Repeat(not
                  Character(Match::eof)
               )
            ) and
            Character(Match::eof);
            
            parser.read(credentials);
            
            if (parser.success() == true)
               _email = parser[0].value();
         }
      }
      
      string base64() const {
         return _inputs[2]->value();
      }
   
      virtual void write(ostream& out) const {
         out << "BasicAuthorization{"
             << _success
             << _value
             << "}" << endl;
      }
      
      virtual const string& email() const {
         return _email;
      }
      
      
   };

}

#endif