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
			   
			  virtual void write(ostream& out)
			  {
			     out << "Range(";
			     Match::write(out);
			     out << ":'";
			     Match::write(out, _minimum);
			     out << "','";
			     Match::write(out, _maximum);
			     out << "')";
			       
			  }
			   
			  virtual std::string name()
			  {
			     return "Range";
			  }
			   
	 };
		

};

#endif
