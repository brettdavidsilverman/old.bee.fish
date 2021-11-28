#ifndef BEE_FISH_PARSER__INVOKE_H
#define BEE_FISH_PARSER__INVOKE_H

#include <iostream>
#include <string>
#include "../misc/optional.h"
#include <map>
#include <sstream>
#include <functional>

using namespace std;

namespace BeeFishParser {

   
   
   class Invoke : public Match
   {
   public:
   
      typedef std::function<void(Match*)> Function;
      Match* _match;
      Function _function;

   public:
   
      Invoke() {
         _match = nullptr;
      }

      Invoke(Match* match) : _match(match) {

      }

      Invoke(
         Match* match,
         Function func
      ) :
         _match(match),
         _function(func)
      {
      }

      virtual ~Invoke() {
         if (_match)
            delete _match;
      }

      virtual bool match(const Char& character) {
         bool matched = _match->matchCharacter(character);
         if (_match->_result == true)
            success();
         else if (_match->_result == false)
            fail();
         return matched;
      }
      virtual void success()
      {
         Match::success();
         if (_function)
            _function(_match);
         
      }
      
   
   };



}

#endif


