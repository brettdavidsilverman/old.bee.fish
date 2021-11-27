#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H

#include <iostream>
#include <string>
#include "../misc/optional.h"
#include <sstream>
#include "match.h"

using namespace std;

namespace BeeFishParser {
   
   class Capture : public Match
   {
   public:
      Match* _match;
      BString& _valueRef;
      BString _value;

   public:
      Capture() :
         _match(nullptr),
         _valueRef(_value) 
      {
      }
      
      Capture(Match* match) :
         _match(match),
         _valueRef(_value)
      {
      }
            
      Capture(
         Match* match,
         BString& value
      ) :
         _match(match),
         _valueRef(value)
      {
      }
      
      virtual ~Capture() {
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

      virtual void capture()
      {
         _valueRef.push_back(_match->character());
      }
      
      virtual const BString& value() const
      {
         return _valueRef;
      }

   };

}

#endif