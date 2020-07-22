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
         new Character(Match::eof)
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
            
            if (credentials.success())
            {
               _email = credentials.email();
               _password = credentials.password();
            }
            else {
               set_success(false);
            }
         }
      }
      
      
      virtual void write(ostream& out) const {
         out << "BasicAuthorization{"
             << success()
             << "}" << endl;
      }
     
      const string& base64() const
      {
         return (*this)[2].value();
      }
      
      const string& email() const
      {
         return _email;
      }
      
      const string& password() const
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
               new Repeat<_Not<Char<Match::eof> > >,
               new Character(Match::eof)
            )
         {
            read(value);
         }
         
         virtual const string& email() const
         {
            return (*this)[0].value();
         }
         
         virtual const string& password() const
         {
            return (*this)[2].value();
         }
         
      };
      

   };

}

#endif