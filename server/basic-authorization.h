#ifndef BEE_FISH_SERVER__BASIC_AUTHORIZATION_H
#define BEE_FISH_SERVER__BASIC_AUTHORIZATION_H

#include "request.h"
#include "base64.h"
#include "server.h"
#include "token.h"

using namespace bee::fish::parser;

namespace bee::fish::server {

   class BasicAuthorization :
      public And {
   protected:
      string _email;
      string _password;
   public:
      BasicAuthorization(
         const string& value
      ) : And(
         new CIWord("Basic"),
         new Blanks(),
         new Base64(),
         new Character(Match::endOfFile)
      )
      {
         read(value);
         
         if (success()) {
         
            string _base64 = base64();
            
            string creds =
               base64::decode(_base64);
               
            Credentials credentials(
               creds
            );
            
            base64().clear();
            
            if (credentials.success())
            {
               _email = credentials.email();
               _password = credentials.password();
               credentials.email().clear();
               credentials.password().clear();
            }
            else {
               success() = false;
            }
         }
      }
      
      
      virtual void write(ostream& out) const {
         out << "BasicAuthorization{"
             << success()
             << "}" << endl;
      }
     
      string& base64()
      {
         return (*this)[2].value();
      }
      
      string& email()
      {
         return _email;
      }
      
      string& password()
      {
         return _password;
      }
      
      
   private:
      class Credentials : public And
      {
      public:
         Credentials(const string& value) : 
            And(
               new And(
                  new Repeat<_Not<Char<'@'>>>(),
                  new Character('@'),
                  new Repeat<_Not<Char<':'> >  >()
               ),
               new Character(':'),
               new Repeat<_Not<Char<Match::endOfFile> > >,
               new Character(Match::endOfFile)
            )
         {
            read(value);
         }
         
         virtual string& email()
         {
            return (*this)[0].value();
         }
         
         virtual string& password()
         {
            return (*this)[2].value();
         }
         
      };
      

   };

}

#endif