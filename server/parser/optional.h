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
   Match* _match;
   
public:
   Optional(Match* match)
      : Match()
   {
      _match = match;
   }
   
   virtual ~Optional() {
      if (_match) {
         delete _match;
         _match = NULL;
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
         _match->match(
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
   
   virtual Match& match()
   {
      return *_match;
   }
   
   virtual string& value()
   {
      if (success() == true)
         return _match->value();
         
      return Match::value();
   }

   
   virtual void write(ostream& out)
   {
      out << "Optional(";
      Match::write(out);
      out << *_match;
      out << ")";
   }
   
};


}

#endif
