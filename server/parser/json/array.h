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
               new LazyLoadJSON(this),
               new Optional(
                  new Records(this)
               )
            )
         ),
         new Character(']')
      )
      {
      }
      
      class Record : public And
      {
      public:
         Record(Array* array = NULL) : And(
            new Character(','),
            new LazyLoadJSON(array)
         )
         {
         }
      };
      
      class Records : public Repeat<Record>
      {
      protected:
         Array* _array;
      public:
         Records(Array* array) : 
            Repeat()
         {
            _array = array;
         }
         
         virtual Record* createItem()
         {
            return new Record(_array);
         }
      };
      
      class LazyLoadJSON :
         public LazyLoad<JSON>
      {
      protected:
         Array* _array;
      public:
         LazyLoadJSON(Array* array) :
            LazyLoad()
         {
            _array = array;
         }
         
         virtual JSON* createItem();
      };
      
      
      virtual void write(ostream& out);
         
   };
}

#endif