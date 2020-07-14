#ifndef BEE_FISH_PARSER__CHARACTER_H
#define BEE_FISH_PARSER__CHARACTER_H

#include "match.h"

namespace bee::fish::parser {

class Character : public Match {
private:
   int _character;

public:
   Character(int character) :
      Match(),
      _character(character)
   {
   }
   
   Character(const Character& source) :
      Match(source),
      _character(source._character)
   {
   }

   virtual bool
   match(int character)
   {
      bool matched =
         (_character == character);
         
      if (matched) {
         Match::match(character);
         set_success(true);
         
      }
      else {
         set_success(false);
      }
      
      return matched;
   }
   
   virtual void write(ostream& out) const {
      out << "Character('" 
          << (char)_character
          << "')";
   }
   
   virtual Match* copy() {
      Character* copy =
         new Character(*this);
      return copy;
   }
   
};


};

#endif
