#ifndef BEE_FISH___JSON_H
#define BEE_FISH___JSON_H

#include "../parser/parser.h"
#include "version.h"
#include "blank-space.h"
#include "number.h"
#include "utf-8.h"
#include "string.h"
#include "array.h"
#include "object.h"
#include "boolean.h"
#include "null.h"
#include "set.h"


using namespace bee::fish::parser;

namespace bee::fish::json
{

   extern Match JSON;
   
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
      
   Match JSON = _JSON;
}

#endif