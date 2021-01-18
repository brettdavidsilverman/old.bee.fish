#ifndef BEE_FISH_SERVER__BASIC_AUTHORIZATION_H
#define BEE_FISH_SERVER__BASIC_AUTHORIZATION_H

#include "../parser/parser.h"
#include "base64.h"
#include "request.h"

using namespace bee::fish::parser;

namespace bee::fish::server {

   class BasicAuthorization :
      public And {
   protected:
      string _username;
      string _password;
   public:
      BasicAuthorization(
         const string& value
      ) : And(
         new CIWord("Basic"),
         new Blanks(),
         new Base64(),
         new Character(Match::EndOfFile)
      )
      {
         _inputs[2]->_capture = true;
         
         if (read(value, true))
         {
         
            string _base64 = base64();
            string creds =
               base64::decode(_base64);
            Credentials credentials(
               creds
            );
            
            base64().clear();
    
            if (credentials.result())
            {
               _username = credentials.username();
               _password = credentials.password();
               credentials.username().clear();
               credentials.password().clear();
               success();
            }
            else
            {
               fail();
            }
         }
      }
      
      
      virtual void write(ostream& out) {
         out << "BasicAuthorization("
             << result()
             << ")" 
             << endl;
      }
     
      string& base64()
      {
         return _inputs[2]->value();
      }
      
      string& username()
      {
         return _username;
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
               new Repeat<bee::fish::parser::_Not<Char<':'> >  >(),
               new Character(':'),
               new Repeat<bee::fish::parser::_Not<Char<Match::EndOfFile> > >,
               new Character(Match::EndOfFile)
            )
         {
            _inputs[0]->_capture = true;
            _inputs[2]->_capture = true;
            read(value, true);
         }
         
         virtual string& username()
         {
            return _inputs[0]->value();
         }
         
         virtual string& password()
         {
            return _inputs[2]->value();
         }
         
      };
      

   };

}

#endif