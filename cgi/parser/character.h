#include "match.h"

namespace Bee::Fish::Parser {

class Character : public Match {
private:
   int _character;

public:
   Character(int character)
   {
      _character = character;
      cout << "Character::Character(";
      if (character == Match::eof)
         cout << "eof";
      else
         cout << "'" 
              << (char)character
              << "'";
      cout << ")"
           << endl;
   }
   
   Character(const Character& source) :
      Match(source)
   {
      _character = source._character;
      cout << "Character::copy()" << endl;
   }

   virtual optional<bool>
   match(int character)
   {
       cout << "@";
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
      out << "Character";
   }
   
   virtual Match* copy() {
      Character* copy =
         new Character(*this);
      return copy;
   }
   
};


};
