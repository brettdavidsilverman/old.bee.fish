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
   {
      _minimum = minimum;
      _maximum = maximum;
   }
   
   Range(const Range& source) :
      Match()
   {
      _minimum = source._minimum;
      _maximum = source._maximum;
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
         set_success(true);
      }
      else {
         set_success(false);
      }

      return matched;
   }
   
   virtual Match* copy() const {
      Range* copy = new Range(*this);
      return copy;
   }
   
   virtual void write(ostream& out) const
   {
      out << "Range('" 
         << _minimum << "', '"
         << _maximum << "'";
         
      Match::write(out);
      
      out << ")";
       
   }
   
};


};

#endif
