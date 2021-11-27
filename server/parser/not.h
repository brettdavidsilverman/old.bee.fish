#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace BeeFishParser {

   class Not : public Match {
   protected:
      Match* _match;
      
   public:

      Not() : Match() {
         _match = nullptr;
      }

      Not(Match* match)
         : Match()
      {
         _match = match;
      }
      
      virtual ~Not() {
         if (_match)
            delete _match;
      }

      virtual bool match(const Char& character)
      {
      
         bool matched =
            _match->matchCharacter(character);
         
         if (_match->_result == false)
            success();
         else if (_match->_result == true)
            fail();
         
         return !matched;
      
      }
   
   };
   
};

#endif
