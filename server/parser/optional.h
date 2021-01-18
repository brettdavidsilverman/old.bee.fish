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
      Optional(Match* match)
         : Match()
      {
         _item = match;
      }
   
      virtual ~Optional()
      {
         if (_item) {
            delete _item;
            _item = NULL;
         }
      }
   
		   virtual bool match(int character)
		   {
		     
		      bool matched =
		         item().match(character);
		      
		      
		      if (item().result() == true) {
		         success();
		         _matched = true;
		      } 
		      else if (item().result() == false) {
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
   
   };


}

#endif
