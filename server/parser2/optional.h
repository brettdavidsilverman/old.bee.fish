#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace bee::fish::parser {

   using namespace std;

   class Optional : public Match {
   protected:
      Match* _item;
      bool _matched = false;
   
   public:
      Optional(const Match& match) :
         Match(),
         _item(match.copy())
      {
      }
     
      Optional(const Optional& source) :
         _item(source._item->copy())
      {
      }
      
      virtual ~Optional()
      {
         delete _item;
      }
      
      friend Optional operator ~(const Match& match);
      
		   virtual bool match(int character)
		   {
		     
		      bool matched =
		         _item->match(character);
		      
		      
		      if (_item->result() == true) {
		         success();
		         _matched = true;
		      } 
		      else if (_item->result() == false) {
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
		      return *_item;
		   }
		   
		   virtual string& value()
		   {
		      if (result() == true)
		         return item().value();
		         
		      return Match::value();
		   }
		   
		   virtual string name()
		   {
		      return "Optional";
		   }
		   
		   virtual bool isOptional()
		   {
		      return true;
		   }
      
      virtual Match* copy() const
      {
         return new Optional(*this);
      }
   
   };


}

#endif
