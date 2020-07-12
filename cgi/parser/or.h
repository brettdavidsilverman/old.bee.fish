#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H

#include "parser.h"

namespace bee::fish::parser {

class Or : public Match {
private:
   Match* _item = NULL;
   
public:

   Or(initializer_list<Match*> input) :
      Match(input)
   {
   }
   
   Or(const Or& source) :
      Match(source),
      _item(NULL)
   {
   }
   
   virtual optional<bool>
   match(int character)
   {
      bool matched = false;
      
      for (auto
           it  = _inputs.begin();
           it != _inputs.end();
           ++it)
      {
         Match* item = *it;
         
         if (item->success() == nullopt) {
            
            if (item->match(character) != false) {
               Match::match(character);
               matched = true;
            }
            
            if (item->success() == true) {
               _item = item;
               _success = true;
               return true;
            }
            
         }
      }
      
      if (!matched)
         _success = false;
         
      return _success;
   }
   
   
   virtual Match* item() const {
      return _item;
   }
   
   virtual const string& value() const {
      return item()->value();
   }
   
   virtual void write(ostream& out) const {
      out << "Or";
   }
   
   virtual Match* copy() {
      Or* copy = new Or(*this);
      return copy;
   }
   
};


};

#endif
