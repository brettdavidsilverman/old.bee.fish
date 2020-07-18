#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <experimental/optional>
#include "match.h"

using namespace std;

namespace bee::fish::parser {

   class And : public Match {
   protected:
   
      size_t _index;
      vector<Match*> _inputs;
      vector<Match*>::iterator
         _iterator;

   public:

      template<typename ...T>
      And(T*... inputs) :
         _inputs{inputs...}
      {
         _iterator = _inputs.begin();
      }
   
      And(const And& source) :
         Match()
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
      
        _iterator = _inputs.begin();

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
      
 
         Match* item = (*_iterator);

         bool matched =
            item->match(character);
         
         if (matched)
            Match::match(character);

         if (item->success() == true) {
            ++_index;
            if (++_iterator ==
                  _inputs.end())
            {
               set_success(true);
            }

         }
         else if (item->success() == false)
            set_success(false);
         
         if (!matched && success() == nullopt)
            matched = match(character);
            
         return matched;
      }
   
      virtual vector<Match*>& inputs()
      {
         return _inputs;
      }
   
      virtual void write(ostream& out) const
      {
         out << "And(";
      
         Match::write(out);
      
         for (auto it = _inputs.cbegin();
                it != _inputs.cend();
              ++it)
         {
            out << **it;
         }
      
         out << ")";
      }
   
      virtual Match* copy() const {
         And* copy = new And(*this);
         return copy;
      }
   
      virtual Match&
      operator [] (size_t index) const {
         if (success() != true)
            throw
               std
               ::experimental
               ::bad_optional_access
               ("And::[]");

         if (index >= _inputs.size())
            throw std::out_of_range
               ("And::[]");
               
         return *(_inputs[index]);
      }
   
   };

};

#endif
