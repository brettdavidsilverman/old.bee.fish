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
   
   virtual bool match(char character) {
      bool matched = false;
 
      vector<Match*> _inputs = inputs();
  
      for (size_t i = 0;
           i < _inputs.size();
           i++)
      {
         Match* item = _inputs[i];

         if (item->success() == nullopt) {
         
            if (item->match(character))
               matched = true;
            
            if (item->success() == true) {
               _item = item;
               _index = i;
               setSuccess(true);
               return matched;
            }
            
         }
      }
      
      if (!matched)
         setSuccess(false);
         
      return matched;
   }
   
   virtual void readEnd() {
      Match::readEnd();
      vector<Match*> _inputs = inputs();
      
      for (size_t i = 0;
           i < _inputs.size();
           i++)
      {
         Match* item = _inputs[i];
         if (item->success() != false) {
            item->readEnd();
            if (item->success() == true) {
               _item = item;
               _index = i;
               setSuccess(true);
               break;
            }
         }
      }

   }
   
   Match& item() {
      return *_item;
   }
   
   virtual string value() const {
      return _item->value();
   }
};



};
