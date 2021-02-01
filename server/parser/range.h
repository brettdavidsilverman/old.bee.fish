#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"

namespace bee::fish::parser {

            
   class Range : public Match {
   private:
			  char _minimum;
			  char _maximum;
			   
		public:
			  Range(char minimum, char maximum) :
			     Match("Range"),
			     _minimum(minimum),
			     _maximum(maximum)
			  {
			  }
			  
			  Range(const Range& source) :
			     Match(source.name()),
			     _minimum(source._minimum),
			     _maximum(source._maximum)
      {
      }
      
			  virtual bool match(int character)
			  {
			   
			     if (character == Match::EndOfFile)
			        return false;
			         
			     bool matched =
			        (_minimum <= character) &&
			        (_maximum >= character);
			         
			     if (matched)
			     {
			        success();
			        Match::match(character);
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
			   
			  virtual void write(ostream& out) const
      {
         Match::write(out);
         
         out << "('";
         
         Match::write(out, _minimum);
         
         out << "', '";
         
         Match::write(out, _maximum);
         
         out << "')";
      }
	 };
		

};

#endif
