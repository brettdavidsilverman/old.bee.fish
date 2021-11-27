#ifndef BEE_FISH_JSON__BLANK_SPACE_H
#define BEE_FISH_JSON__BLANK_SPACE_H

#include "../parser/parser.h"

using namespace BeeFishParser;

namespace BeeFishJSON
{
   
   const Label BlankSpaceCharacter =
      Label(
         "BlankSpaceCharacter",
         new Or(
            new BeeFishParser::
               Character(0x0020),
               
            new BeeFishParser::
               Character(0x000A),
               
            new BeeFishParser::
               Character(0x000D),
               
            new BeeFishParser::
               Character(0x0009),
               
            new BeeFishParser::
               Character('\r'),
               
            new BeeFishParser::
               Character('\n')
         )
      );
      
   inline Label BlankSpace = Label(
      "BlankSpace",
      new Repeat(BlankSpaceCharacter.copy())
   );
   
   
}

#endif