#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match {
   protected:
   public:

      Not(MatchPtr match) :
         Match(match)
      {
      }
   
      Not(const Not& source) :
         Match(source)
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
            capture(character);

         if (_match->result() == false)
            success();
         else if (_match->result() == true)
            fail();
         else if (character == BString::EndOfFile) {
            success();
         }
         
            
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
             << *_match
             << ")";
      }
      
      virtual BString value() const
      {
         return _value;
      }
   };
   
};

#endif
