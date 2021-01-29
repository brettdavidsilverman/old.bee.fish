#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      Match* _template;
      Match* _match = NULL;
			  
   protected:
			  //vector<Match*> _items;
			  size_t _matchedCount = 0;
			  
   public:
			
      Repeat(const Match& t) :
         Match(),
         _template(t.copy())
			  {
			  }
			  
			  Repeat(const Repeat& source) :
			     _template(source._template->copy())
      {
      }
			   
			  virtual ~Repeat() {
			  
			     if (_match)
			     {
			        delete _match;
			        _match = NULL;
			     }
			     
			     delete _template;
			    
			  }
			   
			   
			  virtual bool match(int character)
			  {
			
			     if (_match == NULL)
			        _match = createItem();
			         
			     bool matched =
			        _match->match(character);
			         
			     if (matched)
			        Match::match(character);
			      
			     if (_match->result() == true)
			     {
			      
			        addItem(_match);
			         
			        _match = createItem();
			         
			        ++_matchedCount;
			     }
			     else if (
			           (_match->result() == false) ||
			           (!matched) ||
			           (character == Match::EndOfFile)
			        )
			     {
			        if (_matchedCount > 0)
			        {
			           success();
			        }
			        else
			        {
			           fail();
			        }
			         
			     }
			      
			     return matched;
			      
			  }

			  virtual Match* createItem() {
			     Match* copy = _template->copy();
			     return copy;
			  }
			   
			  virtual void addItem(Match* match)
			  {
			     delete match;
			  }
			   
			   
			  virtual string name()
			  {
			     return
			        "Repeat" + 
			        to_string(_matchedCount);
			  }
			   
			  virtual Match* copy() const
      {
         return new Repeat(*this);
      }
   };


};

#endif
