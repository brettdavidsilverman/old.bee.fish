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
   
   Range(const Range& source) :
      Match(source)
   {
      _minimum = source._minimum;
      _maximum = source._maximum;
   }
   
   virtual optional<bool>
   match(int character)
   {
   
      if (character == Match::eof)
         return false;
         
      bool matched =
         (_minimum <= character) &&
         (_maximum >= character);
         
      if (matched) {
         Match::match(character);
         _success = true;
      }
      else {
         _success = false;
      }

      return _success;
   }
   
   virtual Match* copy() {
      Range* copy = new Range(*this);
      return copy;
   }
   
   
};


};
