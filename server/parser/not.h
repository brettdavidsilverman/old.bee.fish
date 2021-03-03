#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match{
   protected:
      MatchPtr _item;
   public:

      Not(MatchPtr match) :
         _item(match)
      {
      }
   
      Not(const Not& source) :
         _item(source._item)
      {
      }

      virtual ~Not()
      {
      }
      
      virtual bool match(Char character)
      {
      
         bool matched =
            _item->match(character);
         
         if (_item->result() == false)
            success();
         else if (_item->result() == true)
            fail();
         else if (character == BString::EndOfFile) {
            success();
         }
         
         if (!matched)
            capture(character);
            
         return !matched;
      
      }
   
      virtual MatchPtrBase copy() const
      {
         return make_shared<Not>(*this);
      }
   
      virtual void write(ostream& out) const
      {
      
         out << "Not";
         
         writeResult(out);
         
         out << "("
             << *_item
             << ")";
      }
   };
   
};

#endif
