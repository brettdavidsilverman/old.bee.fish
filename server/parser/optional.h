#pragma once
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
   
public:
   Optional(Match* match)
      : Match()
   {
      _item = match;
      _item->setParent(this);
   }
   
   virtual ~Optional()
   {
      if (_item) {
         _item->setParent(NULL);
         delete _item;
         _item = NULL;
      }
   }
   
   virtual void removeChild(Match* child)
   {
      if (_item == child)
      {
         _item->setParent(NULL);
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

   
   virtual void write(ostream& out)
   {
      out << "Optional(";
      Match::write(out);
      out << item();
      out << ")";
   }
   
};


}

#endif
