#ifndef BEE_FISH__JSON_H
#define BEE_FISH__JSON_H

#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "string.h"
#include "array.h"
#include "object.h"
#include "boolean.h"
#include "null.h"
#include "set.h"
#include "version.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{

   Match JSON = Word("undefined");
   
   const Match _JSON =
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