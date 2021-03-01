#ifndef BEE_FISH_PARSER__M_H
#define BEE_FISH_PARSER__M_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"
#include "match-ptr.h"

using namespace std;

namespace bee::fish::parser {

   class M : public Match
   {
   public:
      MatchPtr _item;
      
   public:
   
      M()
      {
      }
      
      M(const MatchPtr& item) :
         _item(item)
      {
      }
      
      M(const M& source) :
         _item(source._item->copy())
      {
      }
      
      virtual bool match(Char character)
      {
         return match(character, *_item);
      }
      
      bool match(Char character, Match& item)
      {
         bool matched =
            item.match(character);
         
         if (item._result == true)
            success();
         else if (item._result == false)
            fail();
               
         return matched;
      }


      virtual MatchPtr copy() const
      {
         return new M(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "M";
         writeResult(out);
         out << "()";
      }
      
      
   };


};

#endif
