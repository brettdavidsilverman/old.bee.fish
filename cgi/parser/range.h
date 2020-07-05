#include "match.h"

namespace Bee::Fish::Parser {

            
class Range : public Match {
protected:
   char _minimum;
   char _maximum;
public:
   Range(char minimum, char maximum) {
      _minimum = minimum;
      _maximum = maximum;
   }
   
   virtual bool match(int character)
   {
   
      bool matched =
         (_minimum <= character) &&
         (_maximum >= character);
         
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
