#ifndef BEE_FISH_SERVER__BASIC_AUTHORIZATION_H
#define BEE_FISH_SERVER__BASIC_AUTHORIZATION_H

#include "../parser/parser.h"
#include "base64.h"
#include "request.h"

using namespace bee::fish::parser;

namespace bee::fish::server {

   class BasicAuthorization : public And{
   protected:
      string _username;
      string _password;
      string _base64;
         
   public:
      BasicAuthorization(
         const string& value
      ) : And(
         CIWord("Basic") and
         Blanks() and
         Capture(
            Base64(),
            [this](Capture& item)
            {
               this->_base64 = item.value();
            }
         ) and
         Character(Match::EndOfFile)
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
      protected:
         string _username;
         string _password;
      public:
         Credentials(const string& value) : 
            And(
               Capture(
                  Repeat(
                     not Character(':')
                  ),
                  [this](Capture& item)
                  {
                     this->_username =
                        item.value();
                  }
               ) and
               Character(':') and
               Capture(
                  Repeat(
                     not Character(
                        Match::EndOfFile
                     )
                  ),
                  [this](Capture& item)
                  {
                     this->_password =
                        item.value();
                  }
               ) and
               Character(Match::EndOfFile)
            )
         {
            read(value);
         }
         
         virtual string& username()
         {
            return _username;
         }
         
         virtual string& password()
         {
            return _password;
         }
         
      };
      

   };

}

#endif