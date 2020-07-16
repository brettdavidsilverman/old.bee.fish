#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>

#include "match.h"

using namespace std;

namespace bee::fish::parser {

class Or : public Match {
protected:
   Match* _item = NULL;
   vector<Match*> _inputs;
   
public:

   template<typename ...T>
   Or(T*... inputs) :
      _inputs{inputs...}
   {
   }
   
   Or(const Or& source) :
      Match(source),
      _item(NULL)
   {
      for (auto
             it = source._inputs.cbegin();
             it != source._inputs.cend();
           ++it)
      {
         _inputs.push_back(
            (*it)->copy()
         );
      }
   }
   
   virtual ~Or() {
      for (auto
             it = _inputs.cbegin();
             it != _inputs.cend();
           ++it)
      {
         delete (*it);
      }
   }
   
   virtual bool
   match(int character)
   {
   
      bool matched = false;

      for ( auto
              it  = _inputs.begin();
              it != _inputs.end();
            ++it )
      {
         Match* item = *it;
        
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
   
   
   virtual Match& item() const {
      return *_item;
   }
   
   virtual const string& value() const {
      return item().value();
   }
   
   friend ostream& operator <<
   (ostream& out, const Or&  match)
   {
      out << "Or";
      out << (Match&)(match);
      
      return out;
   }
   
   virtual Match* copy() const {
      Or* copy = new Or(*this);
      return copy;
   }
   
   virtual Match&
   operator [] (size_t index) {
      return *(_inputs[index]);
   }
   
};


};

#endif
