#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include <vector>
#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      Match* _template;
      vector<Match*> _items;
      
   public:
      size_t _minimum = 1;
      size_t _maximum = 0;
      size_t _matchedCount = 0;
   public:
			
      Repeat(
         Match* template_,
         size_t minimum = 1,
         size_t maximum = 0
      ) :
         _template(template_),
         _minimum(minimum),
         _maximum(maximum)
      {
         _capture = true;
			  }
			  
			  Repeat(const Repeat& source) :
			     Match(source),
			     _template(source._template->copy()),
			     _minimum(source._minimum),
			     _maximum(source._maximum)
      {
      }
			   
			  virtual ~Repeat()
			  {
			     delete _template;
			     
			     for (Match* item : _items)
			        delete item;
			  }
			  
			  virtual bool match(const Char& character)
			  {
			
			     if (!_match)
			        _match = createItem();
			         
			     bool matched =
			        _match->match(character);

			     if (_match->_result == true)
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
			        
			        
			        if (_matchedCount == _maximum)
			           success();
			           
			        if (matched)
			           capture(character);
			        

			     }
			     else if (
			           (_match->_result == false) ||
			           (!matched)
			        )
			     {
			        if (matched)
			           capture(character);
			        

			        if (_matchedCount >= _minimum)
			        {
			           success();
			        }
			        else
			        {
			           matched = false;
			           fail();
			        }
			        
			         
			     }
			     
			     return matched;
			      
			  }
			  
			  virtual Match* createItem()
			  {
			     return _template->copy();
			  }
			   
			  virtual void matchedItem(Match* match)
			  {
			     if (_capture)
            _items.push_back(match);
         else
            delete match;
			  }
			   
			  virtual Match* copy() const
      {
         return new Repeat(*this);
      }
      
      virtual const BString& value() const
      {
         return _value;
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex
      ) const
      {
         out << tabs(tabIndex) << "Repeat";
         
         writeResult(out);
         
         out << endl
             << tabs(tabIndex)
             << "("
             << endl;
         _template->write(out, tabIndex + 1);
         out << ", "
             << endl
             << tabs(tabIndex + 1)
             << _matchedCount
             << endl
             << tabs(tabIndex)
             << ")";
      }
      
 
   };


};

#endif
