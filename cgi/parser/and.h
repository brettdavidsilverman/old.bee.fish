#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>

#include "match.h"

using namespace std;

namespace bee::fish::parser {

class And : public Match {
protected:
   
   size_t _index;
   vector<Match*> _inputs;
   vector<Match*>::iterator
      _inputs_iterator;

public:

   template<typename ...T>
   And(T*... inputs) :
      _inputs{inputs...}
   {
      _inputs_iterator = _inputs.begin();
   }
   
   And(const And& source) :
      Match(source)
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
      
     _inputs_iterator = _inputs.begin();

   }
   
   virtual ~And() {
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
      
      Match* item = (*_inputs_iterator);

      bool matched =
         item->match(character);
         
      if (matched)
         Match::match(character);
         
      if (item->success() == true) {
         ++_index;
         if (++_inputs_iterator ==
               _inputs.end())
         {
            set_success(true);
         }

      }
       
      return matched;
   }
   
   virtual vector<Match*>& inputs()
   {
      return _inputs;
   }
   
   friend ostream& operator <<
   (ostream& out, const And& match)
   {
      out << "And";
      out << (Match&)(match);
      
      return out;
   }
   
   virtual Match* copy() const {
      And* copy = new And(*this);
      return copy;
   }
   
   virtual Match&
   operator [] (size_t index) {
      return *(_inputs[index]);
   }
   
};

};

#endif
