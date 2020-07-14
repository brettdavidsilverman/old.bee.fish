#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H

#include "match.h"
#include <stdexcept>

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
      Match& _and = *(_inputs[0]);
      Match& optional = _and[0];
      return optional;
   }
      
   virtual Match& next() {
      Match& _and = *(_inputs[0]);
      if (_and.success() == true)
         return _and[1];
      else
         return *(_inputs[1]);
   }
   
   virtual Match& operator[] (size_t index) {
      switch (index)
      {
      case 0:
         return optional();
      case 1:
         return next();
      default:
          throw std::out_of_range
             ("Optional::[]");
      }
   }
};

}

#endif
