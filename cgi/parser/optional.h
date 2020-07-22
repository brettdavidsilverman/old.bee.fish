#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include "match.h"
#include <stdexcept>

namespace bee::fish::parser {

using namespace std;

class Optional : public Match {
protected:
   Match* _match;
   static const string default_value;
   
public:
   Optional(Match* match)
      : Match()
   {
      _match = match;
   }
   
   virtual ~Optional() {
      delete _match;
   }
   
   virtual bool match(int character) {
   
      bool matched =
         _match->match(character);
         
      optional<bool> success =
         _match->success();
         
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
   
   virtual const string& value() const {
      if (_match->success() == true)
         return _match->value();
      else
         return default_value;
   }

   
   virtual void write(ostream& out) const
   {
      out << "Optional(";
      Match::write(out);
      out << *_match;
      out << ")";
   }
   
};

}

#endif
