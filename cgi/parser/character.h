#include "match.h"

namespace Bee::Fish::Parser {

class Character : public Match {
private:
   int _character;

public:
   Character(int character) {
      _character = character;
   }

   virtual bool match(int character) {
      
      bool matched =
         (_character == character);
         
      if (matched) {
         Match::match(character);
         setSuccess(true);
      }
      else
         setSuccess(false);
         
      return matched;
   }
   
};


};
