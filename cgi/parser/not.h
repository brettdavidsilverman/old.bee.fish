#include "match.h"

namespace Bee::Fish::Parser {

class Not : public Match {
   Match* _match;
public:

   Not(Match* match) {
      _match = match;
   }
   
   virtual ~Not() {
      delete _match;
   }

   virtual bool match(int character) {
      
      bool matched =
         _match->match(character);
         
      if (!matched)
         Match::match(character);
     
      
      if (_match->success() == false)
         setSuccess(true);
      else if (_match->success() == true)
         setSuccess(false);
      else if (character == Match::eof) {
         setSuccess(true);
      }
      
      return !matched;
      
   }
   
};




};
