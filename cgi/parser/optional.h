#include "match.h"

namespace Bee::Fish::Parser {

class Optional : public Or {
public:
   Optional(
      Match* optional,
      Match* next
   ) :
      Or (
         new And(
            optional,
            next
         ),
         next->copy()
      )
   {
   }
      
   virtual Match* optional() {
      Match* _and = (*this)[0];
      Match* optional = (*_and)[0];
      return optional;
   }
      
   virtual Match* next() {
      Match* _and = (*this)[0];
      Match* next;
      if (_and->success() == true)
         next = (*_and)[1];
      else
         next = (*this)[1];
      return next;
   }
      
};

}
