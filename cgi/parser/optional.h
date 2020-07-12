#include "match.h"

namespace Bee::Fish::Parser {

class Optional : public Match {
private:
   Match* _match;


   const string null_string = "";
public:
   Optional(Match* match) {
      _match = match;
   }
   
   virtual ~Optional() {
      delete _match;
   }
   
   virtual optional<bool>
   match(int character)
   {
      optional<bool> matched =
         _match->match(character);
      
      if (_match->success() != nullopt ||
          character == Match::eof)
         _success = true;
      
      if (matched == true)
         Match::match(character);
         
      return matched;
   }
   
   virtual const string& value() const {
      if (_match->success() == true)
         return _match->value();
      return null_string;
   }

   virtual void write(ostream& out) const {
      out << "Optional";
   }
};

/*
template<class Next>
class _Optional : public Or {
public:
   _Optional(
      Match* optional
   ) :
      Or(
         new And(
            optional,
            new Next()
         ),
         new Next()
      )
   {
   }
   
   
   virtual void write(ostream& out) {
      out << "_Optional";
   }

};
*/
}
