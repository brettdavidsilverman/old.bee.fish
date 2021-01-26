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
/*
   const And JSON =
      ~BlankSpace and
      (
         Word("true") or
         Word("false") or
         Word("null") or
         Number or
         String or
         Array or
         Object
      );
*/
   class _JSON : public And
   {
   public:
      _JSON() : And(
         ~BlankSpace,
         Word("true") or
            Word("false") or
            Word("null") or
            Number or
            String or
            Array or
            Object
      )
      {}
      
      virtual void success()
      {
         And::success();
         ++count;
      }
      
      inline static unsigned long count = 0;
   };
   
   const And JSON = _JSON();
}

#endif