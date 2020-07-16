#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include "match.h"
#include <stdexcept>

namespace bee::fish::parser {

using namespace std;

class Optional : public Match {
protected:
   Match* _optional;
   static const string default_value;
   
public:
   Optional(Match* match)
   {
      _optional = match;
   }
   
   Optional(const Optional& source) {
      _optional =
         source._optional->copy();
   }
      
   virtual ~Optional() {
      delete _optional;
   }
   
   virtual bool match(int character) {
   
      bool matched =
         _optional->match(character);
         
      optional<bool> success =
         _optional->success();
         
      if (success == true) {
         set_success(true);
      } 
      else if (success == false) {
         set_success(true);
      }
      else if (character == Match::eof) {
         set_success(true);
      }
      return matched;
   }
   
   virtual const string & value() const {
      if (_optional->success() == true)
         return _optional->value();
      else
         return default_value;
   }
   
   virtual Match* copy() const {
      Optional* copy =
         new Optional(*this);
      return copy;
   }
   
   friend ostream& operator <<
   (ostream& out, const Optional&  match)
   {
      out << "Optional";
      out << (Match&)(match);
      
      return out;
   }
};

}

#endif
