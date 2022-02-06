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
         new CRLF()
         /*
         new Optional(
            new Body()
         )
         */
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
         public:
            HTTPVersion() : And(
               new Word("HTTP/1."),
               new Or(
                  new BeeFishParser::Character('0'),
                  new BeeFishParser::Character('1')
               )
            )
            {

            }
         };

         class StatusCode : public Match {
         public:
            StatusCode() : Match(
               new Repeat<Digit>(3, 3)
            ) 
            {

            }
         public:
            class Digit : public Range {
            public:
               Digit() : Range('0', '9') {

               }
            };
         };


         class ReasonPhrase : public And {
         public:
            ReasonPhrase() : And(
               new Repeat<ReasonPhrase::Character>()
            )
            {

            }
         public:
            class Character : public Not {
            public:
               Character() : Not(
                  new Or(
                     new BeeFishParser::Character('\r'),
                     new BeeFishParser::Character('\n')
                  )
               ) 
               {
                  
               }
            };
         };
         

      };

      class Header : public And {
      public:
         Header() : And(
            new Name(),
            new Optional(new Blanks()),
            new BeeFishParser::Character(':'),
            new Optional(new Blanks()),
            new Value(),
            new CRLF()
         )
         {

         }
      public:
         class Name : public And {
         public:
            Name() : And(
               new Repeat<Name::Character>()
            )
            {

            }
         public:
            class Character : public Not {
            public:
               Character() : Not(
                  new Or(
                     new BeeFishParser::Character(':'),
                     new BeeFishParser::Character('\r'),
                     new BeeFishParser::Character('\n')
                  )
               )
               {

               }
            };

         };

         class Value : public Match {
         public:
            Value() : Match(
               new Repeat<Value::Character>()
            )
            {
            }

         public:
            class Character : public Not {
            public:
               Character() : Not(
                  new Or(
                     new BeeFishParser::Character('\r'),
                     new BeeFishParser::Character('\n')
                  )
               )
               {

               }
            };

         };

      };

      class Headers : public Repeat<Header>  {

      };

      class Body : public And {

      };
      
   };

};

#endif
