#ifndef BEE_FISH_PARSER__NAME_H
#define BEE_FISH_PARSER__NAME_H
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

   public:
   
      Label(
         const string& label,
         const MatchPtr& item
      ) :
         Match(item)
      {
         _label = label;
      }
      
      Label(const Label& source) :
         Match(MatchPtr(source._item->copy())),
         _label(source._label)
      {
      }
      
      virtual Match* copy() const
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
