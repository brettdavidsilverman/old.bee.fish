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
 
      vector<Match*> inputs =
         Match::inputs();
  
      for (size_t i = 0;
           i < inputs.size();
           i++)
      {
         Match* item = inputs[i];

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
      vector<Match*> inputs = 
         Match::inputs();
      
      for (size_t i = 0;
           i < inputs.size();
           i++)
      {
         Match* item = inputs[i];
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
   
   virtual Match& item() const {
      return *_item;
   }
   
   virtual const string value() const {
      return item().value();
   }
};



};
