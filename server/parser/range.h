#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"

namespace bee::fish::parser {

            
class Range : public Match {
private:
   char _minimum;
   char _maximum;
   
public:
   Range(char minimum, char maximum)
      : Match()
   {
      _minimum = minimum;
      _maximum = maximum;
   }
   
   virtual bool
   match(int character)
   {
   
      if (character == Match::eof)
         return false;
         
      bool matched =
         (_minimum <= character) &&
         (_maximum >= character);
         
      if (matched) {
         Match::match(character);
         success() = true;
      }
      else {
         success() = false;
      }

      return matched;
   }
   
   virtual void write(ostream& out) const
   {
      out << "Range(";
      Match::write(out);
      out << ":'";
      writeCharacter(out, _minimum);
      out << "','";
      writeCharacter(out, _maximum);
      out << "')";
       
   }
   
};


};

#endif
