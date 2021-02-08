#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include <map>

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const Match JSON;
   
   class _Object:
      public Match,
      public map<string, string>
   {
   public:
   
      class Field : public Match
      {
      public:
         string _name;
         string _value;
         
         Match FieldMatch =
            Capture(
               String,
               [this](Capture& capture)
               {
               }
            ) and
                  
            ~BlankSpace and
            Character(':') and
            ~BlankSpace and
                  
            Capture(
               LoadOnDemand(JSON),
               [this](Capture& capture)
               {
               }
            );
            
      public:
         Field() : Match(FieldMatch)

         {
         }
         
         Field(const Field& source) :
             Match(FieldMatch),
            _name(source._name),
            _value(source._value)
         {
         }
         
         virtual Match* copy() const
         {
            cerr << "Field::copy()";
            Field* copy = new Field(*this);
            return copy;
         }
      };
         
   public:
    
      _Object() : Match(
         Label(
            "Object",
            Set(
               Character('{'),
               //(
                  Field(),
            //      [this](Capture& capture)
            //      {
             //     }
             //  ),
               Character(','),
               Character('}')
            )
         )
      )
      {
         cerr << "_Object()";
      }

      virtual ~_Object()
      {
      }
      
      virtual Match* copy() const
      {
         return new _Object();
      }
      
   };
   
   const Match Object = Word("*");// _Object();
 
}

#endif