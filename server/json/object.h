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
      
      const Match Field =
         Capture(
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
            ),
            [this](Capture& field)
            {
               emplace(_field, _value);
            }
         );
         
      const Match Seperator = Character(',');
         
      const Match _Set =
         Set(
            Character('{'),
            Field,
            Seperator,
            Character('}')
         );
      Match* _match;
   public:

      _Object()
      {
         _match = _Set.copy();
      }
      
      _Object(const _Object& source)
      {
         _match = _Set.copy();
      }
      
      virtual ~_Object()
      {
         delete _match;
      }
      
      virtual Match* copy() const
      {
         return new _Object(*this);
      }
      
      virtual bool match(int character)
      {
         return Match::match(_match, character);
      }
      
   };
   
   const Match Object = Label("Object", _Object());
 
}

#endif