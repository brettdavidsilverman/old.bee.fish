#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace bee::fish::parser {

   class Character : public Match {
   protected:
      int _character;

   public:
      Character(int character) :
         Match(),
         _character(character)
      {
      }
      
      Character(const Character& source) :
         Match(),
         _character(source._character)
      {
      }
      
      virtual bool match(int character)
      {
         bool matched =
            (_character == character);
         
         if (matched)
         {
            success();
            Match::match(character);
         }
         else
         {
            fail();
         }
      
         return matched;
      }
   
      virtual string name() const
      {
         return "Character";
      }
      
      virtual Match* copy() const
      {
         return new Character(*this);
      }
      
   };

};


#endif
