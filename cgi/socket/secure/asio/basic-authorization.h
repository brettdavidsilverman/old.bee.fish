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
         
            string input =
               base64::decode(base64());
               
            _credentials.read(input);
            
            
         }
      }
      
      string base64() const {
         return _inputs[2]->value();
      }
   
      virtual void write(ostream& out) const {
         out << "BasicAuthorization{"
             << success()
             << value()
             << "}" << endl;
      }
      
      virtual const string& email() const {
         return _credentials.email();
      }
      
      virtual const string& password() const {
         return _credentials.password();
      }
      
   protected:
      class Credentials : public And
      {
      public:
         Credentials() : And(
            new And(
               new Repeat<_Not<Char<'@'>>>(),
               new Character('@'),
               new Repeat<_Not<Char<':'> >  >()
            ),
            new Character(':'),
            new Repeat<_Not<Char<Match::eof> > >,
            new Character(Match::eof)
         )
         {
         }
         
         virtual const string& email() const
         {
            return (*this)[0].value();
         }
         
         virtual const string& password() const
         {
            return (*this)[2].value();
         }
      } _credentials;
   
   };

}

#endif