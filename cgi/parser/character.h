#include "match.h"

namespace Bee::Fish::Parser {

class Character : public Match {
private:
   char _character;

public:
   Character(char character) {
      _character = character;
   }

   virtual bool match(char character) {
      
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
