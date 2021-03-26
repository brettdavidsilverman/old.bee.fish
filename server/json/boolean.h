#ifndef BEE_FISH_JSON__BOOLEAN_H
#define BEE_FISH_JSON__BOOLEAN_H

#include <map>
#include <iomanip>
#include <optional>

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{

   const Label True =
      Label("True", Word("true"));
      
   const Label False =
      Label("False", Word("false"));
      
   const Label Boolean = Label("Boolean",
      True or False
   );
   
}

#endif