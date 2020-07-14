#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <list>

#include "match.h"

using namespace std;

namespace bee::fish::parser {

class And : public Match {
protected:
   list<Match> _inputs;
   size_t _index;
   list<Match>::iterator
      _inputs_iterator;

public:

   template<class ...args>
   And(args... inputs) :
      _inputs(inputs...)
   {
   }
   
   friend And& operator=(nullptr_t) noexcept {
      return *this;
   }
/*
   And(const And& source) :
      Match(source)
   {
      for (auto
             it = source._inputs.cbegin();
             it != source._inputs.cend();
           ++it)
      {
         _inputs.push_back(
            *it
         );
      }
      
     _inputs_iterator = _inputs.begin();

   }
   */
   virtual bool
   match(int character)
   {
      
      Match& item = (*_inputs_iterator);

      bool matched =
         item.match(character);
         
      if (matched)
         Match::match(character);
         
      if (item.success() == true) {
         ++_index;
         if (++_inputs_iterator ==
               _inputs.end())
         {
            set_success(true);
         }

      }
       
      return matched;
   }
   
   virtual list<Match&>& inputs()
   {
      return _inputs;
   }
   
   /*
   friend ostream& operator <<
   (ostream& out, const And& match)
   {
      out << "And";
      out << (Match&)(match);
      
      return out;
   }
   */
   virtual Match* copy() {
      And* copy = new And(*this);
      return copy;
   }
   
};

};

#endif
