#ifndef BEE_FISH_WEB__WEB_RESPONSE_H
#define BEE_FISH_WEB__WEB_RESPONSE_H

#include <map>
#include <vector>
#include <typeinfo>
#include "../parser/parser.h"
#include "../json/json-parser.h"
#include "../json/json.h"
#include "../database/path.h"

using namespace BeeFishParser;
using namespace BeeFishPowerEncoding;
      
namespace BeeFishWeb {

   class WebResponse : public And {
   public:
      WebResponse() : And(
         new StatusLine(),
         new Headers(),
         new CRLF(),
         new Optional(
            new Body()
         )
      )
      {
      
      }

   public:

      class CRLF : public Word {
      public:
         CRLF() : Word("\r\n") {

         }
      };

      class StatusLine : public And {
      public:
         StatusLine() : And(
            new HTTPVersion(),
            new Blanks(),
            new StatusCode(),
            new Blanks(),
            new ReasonPhrase(),
            new CRLF()
         )
         {

         }

      public:

         class HTTPVersion : public And {

         };

         class StatusCode : public And {

         };

         class ReasonPhrase : public And {

         };
         

      };

      class Header : public And {

      };

      class Headers : public Repeat<Header>  {

      };

      class Body : public And {

      };
      
   };

};

#endif
