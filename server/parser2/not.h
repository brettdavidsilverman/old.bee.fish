#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match{
   protected:
      Match* _match;
   public:

      Not(const Match& match)
         : _match(match.copy())
      {
      }
   
      Not(const Not& source)
         : _match(source._match->copy())
      {
      }

      virtual ~Not()
      {
         delete _match;
      }
      
      friend Not operator not(const Match& match);

      virtual bool match(int character)
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
   
      virtual string name()
      {
         return "Not";
      }
      
      virtual Match* copy() const
      {
         return new Not(*this);
      }
   
   };
   
};

#endif
