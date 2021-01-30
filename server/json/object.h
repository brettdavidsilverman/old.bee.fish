#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const Match JSON;
   
   class _Object:
      public Match
   {
   public:
      _Object() : Match(
         Set(
            Character('{') and ~BlankSpace,
            (
               Capture(
                  String
               ) and
               ~BlankSpace and
               Character(':') and
               ~BlankSpace and
               Capture(
                  LoadOnDemand(JSON)
               )
            ),
            (
               ~BlankSpace and
               Character(',') and
               ~BlankSpace
            ),
            ~BlankSpace and Character('}')
         )
      )
      {
      }
      
      virtual Match* copy() const
      {
         return new _Object(*this);
      }
   };
   
   const Match Object = _Object();
 
}

#endif