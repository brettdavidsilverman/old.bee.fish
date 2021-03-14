#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"

namespace bee::fish::parser {

            
   class Range : public Match {
   private:
			  Char _minimum;
			  Char _maximum;
			   
		public:
			  Range(Char minimum, Char maximum) :
			     _minimum(minimum),
			     _maximum(maximum)
			  {
			  }
			  
			  Range(const Range& source) :
			     _minimum(source._minimum),
			     _maximum(source._maximum)
      {
      }
      
			  virtual bool match(Char character)
			  {
			   
			     if (character == BString::EndOfFile)
			        return false;
			         
			     bool matched =
			        (_minimum <= character) &&
			        (_maximum >= character);
			         
			     if (matched)
			     {
			        capture(character);
			        success();
			     }
			     else {
			        fail();
			     }
			
			     return matched;
			  }
			   
			  virtual Match* copy() const
      {
         return new Range(*this);
      }
			   
			  virtual void write(
			     ostream& out,
			     size_t tabIndex = 0
			  ) const
      {
      
         out << tabs(tabIndex) << "Range";
         
         writeResult(out);
         
         out << "('";
         
         BString::writeEscaped(out, _minimum);
         
         out << "', '";
         
         BString::writeEscaped(out, _maximum);
         
         out << "')";
      }
	 };
		

};

#endif
