#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "set.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const Label JSON;
  
   const Label Array = Label(
      "Array",
      new Set(
         new Character('['),
         new LoadOnDemand(JSON),
         new Character(','),
         new Character(']')
      )
   );
      
 
}

#endif