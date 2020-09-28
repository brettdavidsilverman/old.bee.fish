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
   
   virtual bool match
   (
      int character,
      optional<bool>& success
   )
   {
     
      optional<bool> childSuccess = nullopt;
      
      bool matched =
         item().match(
            character, childSuccess
         );
      
      
      if (childSuccess == true) {
         success = true;
         _matched = true;
      } 
      else if (childSuccess == false) {
         success = true;
      }
      else if (character == Match::endOfFile) {
         success = true;
      }
      
      if (success == true)
         onsuccess();
      
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
      if (success() == true)
         return item().value();
         
      return Match::value();
   }
   
   virtual string name()
   {
      return "Optional";
   }
   
   
};


}

#endif
