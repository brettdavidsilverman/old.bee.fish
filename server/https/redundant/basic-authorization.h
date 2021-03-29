#ifndef BEE_FISH_HTTPS__BASIC_AUTHORIZATION_H
#define BEE_FISH_HTTPS__BASIC_AUTHORIZATION_H

#include "../parser/parser.h"
#include "request.h"
#include "credentials.h"

using namespace bee::fish::parser;

namespace bee::fish::https {

   class BasicAuthorization
   {
   protected:
      Credentials* _credentials = nullptr;
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
            
            _credentials = new Credentials(
               decoded
            );
       
            _base64.clear();
    
            _result = _credentials->_result;
 
         }
      }
      
      virtual ~BasicAuthorization()
      {
         if (_credentials)
            delete _credentials;
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
      
      Credentials& credentials()
      {
         return *_credentials;
      }
      
      bool result()
      {
         return _result;
      }
      
   };

}

#endif