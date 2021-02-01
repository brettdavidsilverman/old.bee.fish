#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace bee::fish::parser {

   using namespace std;

   class Optional : public Match {
   protected:
      Match _item;
      bool _matched = false;
   
   public:
      Optional(const Match& match) :
         Match("Optional"),
         _item(match)
      {
      }
     
      Optional(const Optional& source) :
         Match(source._name),
         _item(source._item)
      {
      }
      
      virtual ~Optional()
      {
      }
      
		   virtual bool match(int character)
		   {
		     
		      bool matched =
		         _item.match(character);
		      
		      
		      if (_item.result() == true) {
		         success();
		         _matched = true;
		      } 
		      else if (_item.result() == false) {
		         matched = false;
		         success();
		      }
		      else if (character == Match::EndOfFile) {
		         success();
		      }
		      
		      
		      return matched;
		   }
      
      virtual bool& matched()
		   {
		      return _matched;
		   }
		   
		   virtual Match& item()
		   {
		      return _item;
		   }
      
      virtual Match* copy() const
      {
         return new Optional(*this);
      }
   
      virtual void write(ostream& out) const
      {
         Match::write(out);
         
         out << "("
             << _item
             << ")";
      }
      
   };


}

#endif
