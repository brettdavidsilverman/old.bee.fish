#include "match.h"

namespace Bee::Fish::Parser {

class Optional : public Match {
private:
   Match* _match;
   
public:
   Optional(Match* match) {
      _match = match;
   }
   
   virtual ~Optional() {
      delete _match;
   }
   
   virtual bool match(int character) {
      bool matched =
         _match->match(character);
      
      if (_match->success() != nullopt ||
          character == Match::eof)
         setSuccess(true);
      
      
      return matched;
   }
   
   virtual const string value() const {
      if (_match->success() == true)
         return _match->value();
      return "";
   }
 
};




};
