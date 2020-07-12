#include "match.h"

namespace Bee::Fish::Parser {

class Or : public Match {
private:
   Match* _item = NULL;
   
public:
   template<class... Types>
   Or(Types*... input) :
      Match{ input... }
   {
      _item = NULL;
      cout << "Or::Or("
           << _inputs.size()
           << ")"
           << endl;
   }
   
   Or(const Or& source) :
      Match(source)
   {
      _item = NULL;
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
            
            cout << *item;
               
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
