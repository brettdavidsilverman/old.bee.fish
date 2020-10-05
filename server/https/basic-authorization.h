#ifndef BEE_FISH_SERVER__BASIC_AUTHORIZATION_H
#define BEE_FISH_SERVER__BASIC_AUTHORIZATION_H

#include <parser.h>
#include "base64.h"
#include "request.h"

using namespace bee::fish::parser;

namespace bee::fish::server {

   class BasicAuthorization :
      public And {
   protected:
      wstring _username;
      wstring _password;
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
    
            if (credentials.success())
            {
               _username = credentials.username();
               _password = credentials.password();
               credentials.username().clear();
               credentials.password().clear();
               onsuccess();
            }
            else {
               onfail();
            }
         }
      }
      
      
      virtual void write(ostream& out) {
         out << "BasicAuthorization("
             << success()
             << ")" 
             << endl;
      }
     
      string& base64()
      {
         return _inputs[2]->value();
      }
      
      wstring& username()
      {
         return _username;
      }
      
      wstring& password()
      {
         return _password;
      }
      
      
   private:
      class Credentials : public And
      {
      public:
         Credentials(const string& value) : 
            And(
               new Repeat<_Not<Char<':'> >  >(),
               new Character(':'),
               new Repeat<_Not<Char<Match::endOfFile> > >,
               new Character(Match::endOfFile)
            )
         {
            _inputs[0]->_capture = true;
            _inputs[2]->_capture = true;
            read(value, true);
         }
         
         virtual wstring& username()
         {
            return _inputs[0]->wvalue();
         }
         
         virtual wstring& password()
         {
            return _inputs[2]->wvalue();
         }
         
      };
      

   };

}

#endif