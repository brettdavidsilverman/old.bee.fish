#ifndef BEE_FISH___JSON_H
#define BEE_FISH___JSON_H

#include "../parser/parser.h"
#include "version.h"

#include "blank-space.h"
#include "number.h"
#include "string.h"
#include "array.h"
#include "object.h"
#include "boolean.h"
#include "null.h"
#include "set.h"


using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   class _JSON : public Match
   {
   public:
      _JSON() : Match(
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
         )
      )
      {
      }
      
      _JSON(const _JSON& source) : _JSON()
      {
      }
      
      virtual Match* copy() const
      {
         return new _JSON(*this);
      }
   };
   
   Match JSON = _JSON();
}

#endif