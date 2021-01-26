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

   unsigned long json_count = 0;
   
   const And JSON =
      ~BlankSpace and
     // Capture(
       (
         Word("true") or
         Word("false") or
         Word("null") or
         Number or
         String or
         Array or
         Object/*
         [](Capture& item)
         {
            ++json_count;
         }*/
      );
/*
 
   class _JSON : public And
   {
   
   public:
      inline static unsigned long count = 0;
      _JSON() : And(
         ~BlankSpace,
         Capture (
            Word("true") or
            Word("false") or
            Word("null") or
            Number or
            String or
            Array or
            Object,
            [](Capture& capture)
            {
               ++count;
            }
         )
      )
      {}
      
      
   };
   
   const And JSON = _JSON();
   */
}

#endif