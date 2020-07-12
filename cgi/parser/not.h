#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

class Not : public Match {
   Match* _match;
public:

   Not(Match* match) {
      _match = match;
   }
   
   Not(const Not& source) :
      Match(source)
   {
      _match = source._match->copy();
   }
   
   virtual ~Not() {
      delete _match;
   }

   virtual optional<bool>
   match(int character)
   {
      
      optional<bool> matched =
         _match->match(character);
         
      if (matched == false)
         Match::match(character);
     
      
      if (_match->success() == false)
         _success = true;
      else if (_match->success() == true)
         _success = false;
      else if (character == Match::eof) {
         _success = true;
      }
      
      return _success;
      
   }
   
   virtual void write(ostream& out) const {
      out << "Not";
   }
   
   virtual Match* copy() {
      Not* copy = new Not(*this);
      return copy;
   }
   
   
};




};

#endif
