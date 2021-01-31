#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      const Match _template;
      Match* _match = NULL;
			  
   protected:
      size_t _minimum = 1;
      size_t _matchedCount = 0;
			  
   public:
			
      Repeat(const Match& t, size_t minimum = 1) :
         _template(t),
         _minimum(minimum)
			  {
			  }
			  
			  Repeat(const Repeat& source) :
			     _template(source._template),
			     _minimum(source._minimum)
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
			     }
			     else if (
			           (_match->result() == false) ||
			           (!matched) ||
			           (character == Match::EndOfFile)
			        )
			     {
			        if (_matchedCount >= _minimum)
			        {
			           //cerr << "Succ" << *this << endl;
			           success();
			        }
			        else
			        {
			         //  cerr << "Fail" << *this << endl;
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
			   
			   
			  virtual string name() const
			  {
			     return "Repeat";
			  }
			   
			  virtual Match* copy() const
      {
         return new Repeat(*this);
      }
   };


};

#endif
