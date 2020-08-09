#ifndef BEE_FISH_PARSER_JSON__ARRAY_H
#define BEE_FISH_PARSER_JSON__ARRAY_H

#include "../parser.h"
#include "json.h"

using namespace bee::fish::parser;

namespace bee::fish::parser::json {
   
   class JSON;
   
   class Array :
      public And,
      public vector<JSON*>
   {
      
   public:
      Array() : And(
         new Character('['),
         new Optional(
            new And(
               new LazyLoadJSON(),
               new Optional(
                  new Records()
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
            new LazyLoadJSON()
         )
         {
         }
         
      };
      typedef Repeat<Record> Records;
      typedef LazyLoad<JSON> LazyLoadJSON;
      
   };
}

#endif