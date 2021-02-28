#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match{
   protected:
      MatchPtr _match;
   public:

      Not(const MatchPtr& match) :
         _match(match)
      {
      }
   
      Not(const Not& source) :
         _match(source._match)
      {
      }

      virtual ~Not()
      {
      }
      
      virtual bool match(Char character)
      {
      
         bool matched =
            _match->match(character);
         
         if (!matched)
            Match::match(character);
     
         if (_match->result() == false)
            success();
         else if (_match->result() == true)
            fail();
         else if (character == Match::EndOfFile) {
            success();
         }
         
         return !matched;
      
      }
   
      virtual Match* copy() const
      {
         return new Not(*this);
      }
   
      virtual void write(ostream& out) const
      {
      
         out << "Not";
         
         writeResult(out);
         
         out << "("
             << *_match
             << ")";
      }
   };
   
};

#endif
