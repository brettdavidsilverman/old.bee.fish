#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace bee::fish::parser {

   using namespace std;

   class Optional : public Match {
   protected:
      MatchPtr _item;
      bool _matched = false;
   
   public:
      Optional(MatchPtr match) :
         _item(match)
      {
      }
     
      Optional(const Optional& source) :
         _item(source._item->copy())
      {
      }
      
      virtual ~Optional()
      {
      }
      
		   virtual bool match(Char character)
		   {
		     
		      bool matched =
		         _item->match(character);
		      
		      
		      if (_item->_result == true)
		      {
		      
		         _matched = true;
		         success();
		         
		      } 
		      else if (_item->_result == false)
		      {
		         matched = false;
		         success();
		      }
		      else if (character == BString::EndOfFile)
		      {
		         success();
		      }
		      
		      if (matched)
		         capture(character);
		         
		      return matched;
		   }
      
      virtual bool& matched()
		   {
		      return _matched;
		   }
		   
		   virtual Match& item()
		   {
		      return *_item;
		   }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<Optional>(*this);
      }
   
      virtual void write(ostream& out) const
      {
         out << "Optional";
         
         writeResult(out);
         
         out << "("
             << *_item
             << ")";
      }
      
   };


}

#endif
