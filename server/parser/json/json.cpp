#include "json.h"

namespace bee::fish::parser::json
{
   JSON::JSON(Match* parent) : And(
      new Optional(
         new BlankSpace()
      ),
      new Or(
         new True(),
         new False(),
         new Null(),
         new Number(),
         new String(),
         new Array(),
         new Object()
      ),
      new Optional(
         new BlankSpace()
      )
   )
   {
      _parent = parent;
   }
}
