#ifndef BEE_FISH_PARSER_JSON__ARRAY_H
#define BEE_FISH_PARSER_JSON__ARRAY_H

#include "../parser.h"
#include "json.h"

using namespace bee::fish::parser;

namespace bee::fish::parser::json {
   
   class JSON;
   
   class Array : public And
   {
      
   public:
      Array() : And(
         new Character('['),
         new Optional(
            new And(
               new LazyLoad<JSON>(),
               new Optional(
                  new Repeat<Record>()
               )
            )
         ),
         new Character(']')
      )
      {
      }
      
      virtual string name()
      {
         return "Array";
      }
      
      class Record : public And
      {
      public:
         Record() : And(
            new Character(','),
            new LazyLoad<JSON>()
         )
         {
         }
         
      };

   };
}

#endif