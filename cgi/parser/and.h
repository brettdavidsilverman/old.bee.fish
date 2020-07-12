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

      if (item->success() != nullopt)
      {
         _success = false;
         return false;
      }
      
      optional<bool> matched =
         item->match(character);
    
      if (matched == false) {
         _success = false;
      }
      else {
         Match::match(character);
         if (item->success() == true) {
            if (++_inputs_iterator ==
                _inputs.cend())
               _success = true;
         }
      }
         
      return _success;
   }
   
   virtual const vector<Match*>& items() const {
      return _inputs;
   }
   
   Match* operator[](size_t index) {
      return _inputs[index];
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
