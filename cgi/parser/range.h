#include "match.h"

namespace Bee::Fish::Parser {

            
class Range : public Match {
private:
   char _minimum;
   char _maximum;
   
public:
   Range(char minimum, char maximum)
   {
      _minimum = minimum;
      _maximum = maximum;
   }
   
   virtual bool match(char character) {
   
      bool matched =
         (_minimum <= character) &&
         (_maximum >= character);
         
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
