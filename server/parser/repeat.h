#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      Match _template;
      Match* _match = NULL;
			  
   protected:
      size_t _minimum = 1;
      size_t _maximum = 0;
      size_t _matchedCount = 0;
			  
   public:
			
      Repeat(
         const Match& t,
         size_t minimum = 1,
         size_t maximum = 0) :
         _template(t),
         _minimum(minimum),
         _maximum(maximum)
			  {
			  }
			  
			  Repeat(const Repeat& source) :
			     _template(source._template),
			     _minimum(source._minimum),
			     _maximum(source._maximum)
      {
      }
			   
			  virtual ~Repeat() {
			  
			     if (_match)
			     {
			        delete _match;
			        _match = NULL;
			     }
			    
			  }
			   
			   
			  virtual bool match(int character)
			  {
			
			     if (_match == NULL)
			        _match = createItem();
			         
			     bool matched =
			        _match->match(character);

			      
			     if (_match->result() == true)
			     {
			      
			        matchedItem(_match);
			         
			        _match = createItem();
			         
			        ++_matchedCount;
			        
			        if ( _maximum > 0 &&
			             _matchedCount > _maximum )
			        {
			           matched = false;
			           fail();
			        }
			     }
			     else if (
			           (_match->result() == false) ||
			           (!matched) ||
			           (character == Match::EndOfFile)
			        )
			     {
			        if (_matchedCount >= _minimum)
			        {
			           success();
			        }
			        else
			        {
			           fail();
			        }
			         
			     }
			     
			     if (matched)
			        Match::match(character);
			        
			     return matched;
			      
			  }

			  virtual Match* createItem() {
			     Match* copy = _template.copy();
			     return copy;
			  }
			   
			  virtual void matchedItem(Match* match)
			  {
			     delete match;
			  }
			   
			  virtual Match* copy() const
      {
         return new Repeat(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "Repeat";
         
         writeResult(out);
         
         out << "("
             << _template
             << ", "
             << _minimum
             << ", "
             << _maximum
             << ")";
      }
   };


};

#endif
