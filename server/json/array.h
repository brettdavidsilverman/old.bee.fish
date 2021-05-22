#ifndef BEE_FISH_JSON__ARRAY_H
#define BEE_FISH_JSON__ARRAY_H

#include "../parser/parser.h"
#include "blank-space.h"
#include "number.h"
#include "set.h"

using namespace bee::fish::parser;

namespace bee::fish::json {
   
   extern const Label JSON;
  
   class _Array : public Set
   {
   public:
      size_t _size = 0;
   public:
      _Array() : Set(
         new bee::fish::parser::Character('['),
         new LoadOnDemand(JSON),
         new bee::fish::parser::Character(','),
         new bee::fish::parser::Character(']')
      )
      {
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex)
             << "Array";
         writeResult(out);
         out << endl
             << tabs(tabIndex)
             << "("
             << endl
             << tabs(tabIndex + 1)
             << _size
             << ","
             << endl;
         _match->write(out, tabIndex + 1);
         out << endl
             << tabs(tabIndex)
             << ")";
      }
      
      virtual void matchedSetItem(Match* item)
      {
         ++_size;
         Set::matchedSetItem(item);
      }
   };
   
   const Label Array = Label(
      "Array",
      new _Array()
   );
      
 
}

#endif