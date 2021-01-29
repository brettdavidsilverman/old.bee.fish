#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../parser/parser.h"
#include "blank-space.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const Match JSON;
  
   const Match Array = Set(
      Character('['), //and ~BlankSpace,
      LoadOnDemand(JSON),
      
         //~BlankSpace and
         Character(',') ,
         //~BlankSpace
      
      //~BlankSpace and
      Character(']')
   );
 
}

#endif