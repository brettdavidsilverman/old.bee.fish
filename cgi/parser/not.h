#include "match.h"

namespace Bee::Fish::Parser {

class Not : public Match {
   Match* _match;
   string _value = "";
public:

   Not(Match* match) {
      _match = match;
   }
   
   virtual ~Not() {
      delete _match;
   }
   
protected:
   virtual bool match(char character) {
      
      bool matched =
         _match->match(character);
         
      if (!matched) {
         _value += character;
      }
      
      if (_match->success() == false) {
         setSuccess(true);
      }
      else if (_match->success() == true)
         setSuccess(false);

      return !matched;
      
   }
   
   virtual string value() const {
      return _value;
   }
   
};




};
