#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "set.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern Match JSON;
  
   const Match Array = Label(
      "Array",
      Set(
         Character('['),
         LoadOnDemand(JSON),
         Character(','),
         Character(']')
      )
   );
      
 
}

#endif