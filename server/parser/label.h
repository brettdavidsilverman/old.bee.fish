#ifndef BEE_FISH_PARSER__LABEL_H
#define BEE_FISH_PARSER__LABEL_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"
#include "match-ptr.h"

using namespace std;

namespace bee::fish::parser {

   class Label : public Match
   {
   protected:
      string _label;
      MatchPtr _item;
      
   public:
   
      Label(
         const string& label,
         const MatchPtr& item
      ) :
         _label(label),
         _item(item)
      {
      }
      
      Label(const Label& source) :
         _label(source._label),
         _item(source._item->copy())
      {
      }
      
      virtual bool match(Char character)
      {
         return Match::match(character, *_item);
      }
      
      virtual MatchPtr copy() const
      {
         return new Label(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << _label;
         writeResult(out);
         out << "()";
      }
      
      
   };


};

#endif
