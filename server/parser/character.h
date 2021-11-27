#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace BeeFishParser {

   class Character : public Match {
   protected:
      Char _characterToMatch;
      bool _any;
      
   public:
      Character() :
         _characterToMatch(-1),
         _any(true)
      {
         _setup = true;
      }
      
      Character(const Char& character) :
         _characterToMatch(character),
         _any(false)
      {
         _setup = true;
      }
      
      virtual bool match(const Char& character)
      {
         bool matched =
            ( _any ||
              ( _characterToMatch == character )
            );
         
         if (matched)
         {
            success();
         }
         else
         {
            fail();
         }
      
         return matched;
      }
   };


}


#endif
