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

   virtual optional<bool>
   match(int character)
   {
      if (_character == character) {
         _success = true;
         Match::match(character);
      }
      else {
         _success = false;
      }
      
      return _success;
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
