#ifndef BEE_FISH_JSON__BLANK_SPACE_H
#define BEE_FISH_JSON__BLANK_SPACE_H

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   const MatchPtr BlankSpaceCharacter =
      Character(0x0020) or
      Character(0x000A) or
      Character(0x000D) or
      Character(0x0009) or
      Character('\r') or
      Character('\n');
      
   const Label BlankSpace = Label(
      "BlankSpace",
      Repeat(BlankSpaceCharacter)
   );
   
   
}

#endif