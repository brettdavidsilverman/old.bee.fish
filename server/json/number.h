#ifndef BEE_FISH_JSON__NUMBER_H
#define BEE_FISH_JSON__NUMBER_H

#include "../parser/parser.h"

namespace bee::fish::json {
      
   const Match IntegerCharacter =
      Range('0', '9');
   
   const Match Integer =
      Repeat(IntegerCharacter);
   
   const Match Fraction =
      Character('.') and Integer;
         
   const Match Plus = Character('+');
   const Match Minus = Character('-');
   
   const Match Sign = Plus or Minus;
      
   const Match Exponent =
      (
         Character('E') or
         Character('e')
      ) and
      ~Sign and
      Integer;
      
   const Match Number = Name(
      "Number",
      ~Minus and
      Integer and
      ~Fraction and
      ~Exponent
   );
      
}

#endif