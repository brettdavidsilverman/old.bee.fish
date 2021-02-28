#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser
{
   
   class Repeat : public Match
   {
   private:
      MatchPtr _template;
      MatchPtr _item;
      
   protected:
      size_t _minimum = 1;
      size_t _maximum = 0;
      size_t _itemedCount = 0;
   public:
			
      Repeat(
         const MatchPtr template_,
         size_t minimum = 1,
         size_t maximum = 0
      ) :
         _template(template_),
         _item(),
         _minimum(minimum),
         _maximum(maximum)
      {
			  }
			  
			  Repeat(const Repeat& source) :
			     _template(source._template),
			     _item(),
			     _minimum(source._minimum),
			     _maximum(source._maximum)
      {
      }
			   
			  virtual bool match(Char character)
			  {
			
			     if (!_item)
			        _item = createItem();
			         
			     bool matched =
			        _item->match(character);

			     if (_item->_result == true)
			     {
			      
			        matchedItem(_item);
			         
			        _item = createItem();
			         
			        ++_itemedCount;
			        
			        if ( _maximum > 0 &&
			             _itemedCount > _maximum )
			        {
			           matched = false;
			           fail();
			        }
			     }
			     else if (
			           (_item->_result == false) ||
			           (!matched) ||
			           (character == Match::EndOfFile)
			        )
			     {
			        if (_itemedCount >= _minimum)
			        {
			           success();
			        }
			        else
			        {
			           fail();
			        }
			         
			     }
			     
			     if (matched)
			        capture(character);
			        
			     return matched;
			      
			  }

			  virtual MatchPtr createItem()
			  {
			     return _template->copy();
			  }
			   
			  virtual void matchedItem(MatchPtr match)
			  {
			     match.reset();
			  }
			   
			  virtual MatchPtr copy() const
      {
         return new Repeat(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "Repeat";
         
         writeResult(out);
         
         out << "("
             << *_template
             << ", "
             << _minimum
             << ", "
             << _maximum
             << ")";
      }
   };


};

#endif
