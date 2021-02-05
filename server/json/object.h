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
   protected:
      string _field;
      string _value;

   public:
      _Object() : Match(
         Set(
            Character('{') and ~BlankSpace,
            
           // Capture(
               
               Capture(
                  String,
                  _field
               ) and
                  
               ~BlankSpace and
               Character(':') and
               ~BlankSpace and
                  
               Capture(
                  LoadOnDemand(JSON),
                  _value
               )/*,
               
               [](Capture& item)
               {
                     
                  //cerr << _field << endl;
                 
                 // emplace(_field, _value);
                     
               }
               
            )*/,
            (
               ~BlankSpace and
               Character(',') and
               ~BlankSpace
            ),
            ~BlankSpace and Character('}')
         )
      )
      {
         cerr << "_Object()" << endl;
      }
      
      _Object(const _Object& source)
      {
      }
      
      virtual Match* copy() const
      {
         return new _Object(*this);
      }
      
   };
   
   const Match Object = Name("Object", _Object());
 
}

#endif