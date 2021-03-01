#ifndef BEE_FISH_PARSER__LABEL_H
#define BEE_FISH_PARSER__LABEL_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"
#include "match-ptr.h"
#include "m.h"

using namespace std;

namespace bee::fish::parser {

   class Label : public M
   {
   protected:
      string _label;
   public:
   
      Label(
         const string& label,
         const MatchPtr& item
      ) :
         M(item),
         _label(label)
      {
      }
      
      Label(const Label& source) :
         M(source),
         _label(source._label)
      {
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
