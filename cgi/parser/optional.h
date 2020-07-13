#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H

#include "match.h"

namespace bee::fish::parser {

class Optional : public Or {
public:
   Optional(
      Match* optional,
      Match* next
   ) :
      Or (
         {
            new And(
               {
                  optional,
                  next
               }
            ),
            next->copy()
         }
      )
   {
   }
      
   virtual Match& optional() {
      Match& _and = (*this)[0];
      Match& optional = _and[0];
      return optional;
   }
      
   virtual Match& next() {
      Match& _and = (*this)[0];
      if (_and.success() == true)
         return _and[1];
      else
         return (*this)[1];
   }
      
};

}

#endif
