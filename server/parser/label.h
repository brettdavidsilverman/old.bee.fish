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
   public:
   
      Label(
         const string& label,
         Match* match
      ) :
         _label(label)
      {
         _match = match;
 
      }
      
      Label(const Label& source) :
         Match(source),
         _label(source._label)
      {
      }
      
      virtual Match* copy() const
      {
         return new Label(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex) << _label;
         writeResult(out);
         out << "()";
      }
      
      virtual const BString& value() const
      {
         return _match->value();
      }
      
      
   };


};

#endif
