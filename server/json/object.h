#ifndef BEE_FISH_JSON__OBJECT_H
#define BEE_FISH_JSON__OBJECT_H

#include "../parser/parser.h"
#include "blank-space.h"
#include "string.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const And JSON;
   
   const Set Object(
      Character('{') and ~BlankSpace,
      (
         String and
         ~BlankSpace and
         Character(':') and
         ~BlankSpace and
         LoadOnDemand(JSON)
      ),
      (
         ~BlankSpace and
         Character(',') and
         ~BlankSpace
      ),
      ~BlankSpace and Character('}')
   );
 
}

#endif