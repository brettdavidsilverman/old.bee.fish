#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const Match JSON;
  
   const Match Array = Name(
      "Array",
      Set(
         Character('[') and Optional(BlankSpace),
         LoadOnDemand(JSON),
         (
            Optional(BlankSpace) and
            Character(',') and
            Optional(BlankSpace)
         ),
         Optional(BlankSpace) and Character(']')
      )
   );
      
 
}

#endif