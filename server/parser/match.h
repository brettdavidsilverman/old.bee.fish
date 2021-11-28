#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <bitset>
#include "../b-string/string.h"
#include "../misc/optional.h"

#include "misc.h"

using namespace std;

namespace BeeFishParser {

   typedef BeeFishBString::Character Char;

   class Match {
   public:
      inline static const BString& EmptyString = {};
      inline static unsigned long _matchInstanceCount = 0;

      bool _setup = false;      
      BeeFishMisc::optional<bool> _result = BeeFishMisc::nullopt;
      Char _character;

   public:
   
      Match()
      {
         ++_matchInstanceCount;
      }
      
      virtual ~Match()
      {
         --_matchInstanceCount;
      }
      
   public:
  
      bool matched() const
      {
         return (result() == true);
      }
      

   public:

      virtual void setup() {
         _setup = true;
      }

      virtual bool matchCharacter(
         const Char& character
      )
      {
         if (!_setup)
            setup();

         _character = character;


         bool matched = match(character);

         if (matched)
            capture();

         if (_result == true)
            success();
         else if (_result == false)
            fail();

         return matched;
      }
      
      virtual bool match(const Char& character) = 0;
      
      virtual void success()
      {
         _result = true;
      }
   
      virtual void fail()
      {
         _result = false;
      }
            
      virtual BeeFishMisc::optional<bool> result() const
      {
         return _result;
      }

      virtual const BString& value() const
      {
         return EmptyString;
      }
      
      virtual const Char& character() const {
         return _character;
      }

      virtual void capture()
      {
      }
      
   
   };



}

#endif


