#ifndef BEE_FISH_PARSER__NAME_H
#define BEE_FISH_PARSER__NAME_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"

using namespace std;

namespace bee::fish::parser {

   class Label : public Match
   {
   protected:
      string _label;
      Match _match;
   public:
   
      Label(
         const string& label,
         const Match& match
      ) :
         _label(label),
         _match(match)
      {
         
      }
      
      Label(const Label& source) :
         _label(source._label),
         _match(source._match)
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
      
      virtual bool match(int character)
      {
         bool matched = Match::match(
            &_match, character
         );
            
         return matched;
      }
      

   };


};

#endif
