#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include <map>
#include <iomanip>
#include <optional>

#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "string.h"
#include "array.h"
#include "object.h"
#include "version.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{

   const Match JSON =
      ~BlankSpace and (
         Word("true") or
         Word("false") or
         Word("null") or
         String or
         Number or
         Array or
         Object
      );
         
      
}

#endif