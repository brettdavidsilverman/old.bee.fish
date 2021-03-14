#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match {
   protected:
      Match* _match;
      
   public:

      Not(Match* match)
         : Match()
      {
         _match = match;
      }
   

      virtual ~Not() {
         delete _match;
      }
      
      Not(const Not& source) :
         Match(source)
      {
         _match = source._match->copy();
      }
      
      virtual bool match(Char character)
      {
      
         bool matched =
            _match->match(character);
         
         if (!matched)
            capture(character);
     
      
         if (_match->_result == false)
            success();
         else if (_match->_result == true)
            fail();
         else if (character == BString::EndOfFile) {
            success();
         }
         
         return !matched;
      
      }
   
      virtual Match* copy() const
      {
         return new Not(*this);
      }
   
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
      
         BString tabs = Match::tabs(tabIndex);
         
         out << tabs << "Not";
         
         writeResult(out);
         
         out << "("
             << *_match
             << ")";
             
      }
      
   };
   
};

#endif
