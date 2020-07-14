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

   virtual bool
   match(int character)
   {
      
      bool matched =
         _match->match(character);
         
      if (!matched)
         Match::match(character);
     
      
      if (_match->success() == false)
         set_success(true);
      else if (_match->success() == true)
         set_success(false);
      else if (character == Match::eof) {
         set_success(true);
      }
      
      return !matched;
      
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
