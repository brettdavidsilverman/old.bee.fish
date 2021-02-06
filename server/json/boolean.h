#ifndef BEE_FISH_JSON__BOOLEAN_H
#define BEE_FISH_JSON__BOOLEAN_H

#include <map>
#include <iomanip>
#include <optional>

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{

   const Match True =
      Label("True", Word("true"));
      
   const Match False =
      Label("False", Word("false"));
      
   const Match Boolean = Label("Boolean",
      True or False
   );
   
}

#endif