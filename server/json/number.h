#ifndef BEE_FISH_JSON__NUMBER_H
#define BEE_FISH_JSON__NUMBER_H

#include "../parser/parser.h"

namespace bee::fish::json {
      
   const Range IntegerCharacter('0', '9');
   
   const Repeat Integer(IntegerCharacter);
   
   const And Fraction =
         Character('.') and Integer;
   const Or Sign =
      Character('+') or
      Character('-');
      
   const Match Exponent =
      (
         Character('E') or
         Character('e')
      ) and
      Sign and Integer;
      
   const Match Number =
      ~Sign and
      Integer and
      ~Fraction and
      ~Exponent;
      
}

#endif