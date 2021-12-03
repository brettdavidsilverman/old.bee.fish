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
      Char _character = 0;
      Match* _match = nullptr;

      typedef std::function<void(Match&)> OnSuccess;
      OnSuccess _onsuccess;

   public:
   
      Match()
      {
         _match = nullptr;
         ++_matchInstanceCount;
      }

      Match(Match* match) : _match(match) {
         ++_matchInstanceCount;
      }
      
      virtual ~Match()
      {
         if (_match)
            delete _match;

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
      
      virtual bool match(const Char& character)  {
         
         if (!_match) {
            return false;
         }
         
         bool matched = _match->matchCharacter(character);
         _result = _match->_result;
         return matched;
      };
      
      virtual void success()
      {
         if (_onsuccess)
            _onsuccess(*this);
      }
   
      virtual void fail()
      {
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


