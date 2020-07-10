#include "match.h"

namespace Bee::Fish::Parser {

class Or : public Match {
private:
   Match* _item;
   size_t _index;
   
public:
   template<typename... T>
   Or(T*... input) :
      Match{ input... }
   {
      _item = NULL;
   }
   
   virtual bool match(int character) {
      bool matched = false;
      size_t index = 0;
      for (auto
           it  = _inputs.begin();
           it != _inputs.end();
           ++it, ++index)
      {
         Match* item = *it;
         if (item->success() == nullopt) {
         
            if (item->match(character))
               matched = true;
               
            if (item->success() == true) {
               _item = item;
               _index = index;
               setSuccess(true);
               return matched;
            }
            
         }
      }
      
      if (!matched)
         setSuccess(false);
         
      return matched;
   }
   
   
   virtual Match* item() const {
      return _item;
   }
   
   virtual const string value() const {
      return item()->value();
   }
   
};


};
