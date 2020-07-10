#include "match.h"

namespace Bee::Fish::Parser {

class Character : public Match {
private:
   int _character;

public:
   Character(int character) {
      _character = character;
   }

   virtual optional<bool> match(int character) {
      
      bool matched =
         (_character == character);
         
      if (matched) {
         Match::match(character);
         _success = true;
         return true;
      }
      else {
         _success = false;
         return false;
      }
      
   }
   
};


};
