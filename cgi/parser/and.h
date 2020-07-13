#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H

#include "match.h"
#include <initializer_list>
#include <vector>

using namespace std;

namespace bee::fish::parser {

class And : public Match {
   
public:

   And(initializer_list<Match*> input) :
      Match(input)
   {
   }

   And(const And& source) :
      Match(source)
   {
   }
   
   virtual optional<bool>
   match(int character)
   {
      
      Match* item = (*_inputs_iterator);

      optional<bool> matched =
         item->match(character);
         
      if (matched != false)
         Match::match(character);
         
      if (item->success()) {
         if (++_inputs_iterator ==
               _inputs.cend())
         {
            _success = true;
         }
      }
       
      return matched;
   }
   
   virtual const vector<Match*>& items() const {
      return _inputs;
   }
   
   virtual void write(ostream& out) const {
      out << "And";
   }
   
   virtual Match* copy() {
      And* copy = new And(*this);
      return copy;
   }
   
};

};

#endif
