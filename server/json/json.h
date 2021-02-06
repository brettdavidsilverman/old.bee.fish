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
#include "boolean.h"
#include "set.h"
#include "version.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{
   const Match Null = Label("Null", Word("null"));
   
   const Match JSON =
         Optional(
            BlankSpace 
         )
         and (
            Null or
            Boolean or
            String or
            Number or
            Array or
            Object
         );
}

#endif