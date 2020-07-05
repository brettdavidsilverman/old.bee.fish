#include "match.h"

namespace Bee::Fish::Parser {

class Character : public Match {
protected:
   int _character;
   
public:

   Character(const int character) {
      _character = character;
   }
   
   virtual bool match(int character) {
      bool matched =
         _character == character;
         
      if (matched) {
         _value = (char)character;
         setSuccess(true);
      }
      else
         setFailed(true);
         
      return matched;
   }
};


};
