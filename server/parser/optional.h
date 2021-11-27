#ifndef BEE_FISH_PARSER__OPTIONAL_H
#define BEE_FISH_PARSER__OPTIONAL_H
#include <string>
#include <stdexcept>
#include "match.h"

namespace BeeFishParser {

   using namespace std;

   class Optional : public Match {
   protected:
      bool _matched = false;
      Match* _match;

   public:
      Optional() {
         _match = nullptr;
      }

      Optional(Match* match)
      {
         _match = match;
      }

      virtual ~Optional() {
         if (_match)
            delete _match;
      }
   
      virtual bool match(const Char& character)
      {
         
         bool matched =
            _match->matchCharacter(character);
         
         bool succeeded = false;
         
         if (_match->_result == true)
         {
         
            _matched = true;
            succeeded = true;
            
         } 
         else if (_match->_result == false)
         {
            _matched = false;
            succeeded = true;
         }

         if (succeeded)
            success();
            
         return matched;
      }
   
   };


}

#endif
