#ifndef BEE_FISH_PARSER__RANGE_H
#define BEE_FISH_PARSER__RANGE_H

#include "match.h"

namespace BeeFishParser {

            
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
			Match(),
			_minimum(source._minimum),
			_maximum(source._maximum)
		{
		}
      
		virtual bool match(const Char& character)
		{
				
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
			
			out << _minimum;
			
			out << "', '";
			
			out << _maximum;
			
			out << "')";
		}
	};
		

};

#endif
