#ifndef BEE_FISH_JSON__BLANK_SPACE_H
#define BEE_FISH_JSON__BLANK_SPACE_H

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{
   
   const Label BlankSpaceCharacter =
      Label(
         "BlankSpaceCharacter",
         new Or(
            new Character(0x0020),
            new Character(0x000A),
            new Character(0x000D),
            new Character(0x0009),
            new Character('\r'),
            new Character('\n')
         )
      );
      
   const Label BlankSpace = Label(
      "BlankSpace",
      new Repeat(BlankSpaceCharacter.copy())
   );
   
   
}

#endif