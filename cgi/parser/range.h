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
         set_success(true);
      }
      else {
         set_success(false);
      }

      return matched;
   }
   
   virtual void write(ostream& out) const
   {
      out << "Range(";
      Match::write(out);
      out << ":'";
      write_character(out, _minimum);
      out << "','";
      write_character(out, _maximum);
      out << "')";
       
   }
   
};


};

#endif
