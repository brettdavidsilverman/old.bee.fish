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
   
   virtual bool
   match(int character)
   {
   
      bool matched = false;
      cerr << *this;
      for ( auto
              it  = _inputs.begin();
              it != _inputs.end();
            ++it )
      {
         Match* item = *it;
         cerr << "|" << *item << "|";
         if (item->success() == nullopt) {
       
            if (item->match(character))
               matched = true;
            
            if (item->success() == true) {
               _item = item;
               set_success(true);
               return matched;
            }
            
         }
      }
      
      if (_success == nullopt && !matched) {
         set_success(false);
      }
      
      return matched;
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
