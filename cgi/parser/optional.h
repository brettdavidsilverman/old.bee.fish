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
   
   virtual bool match(char character) {
      bool matched =
         _match->match(character);
      
      if (_match->success() != nullopt)
         setSuccess(true);
      
      return matched;
   }
   
   virtual void readEnd() {
      _match->readEnd();
      setSuccess(true);
   }
   
   virtual const string value() const {
      if (_match->success() == true)
         return _match->value();
      return "";
   }
 
};




};
