#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include "../misc/debug.h"
#include "../misc/optional.h"
#include <sstream>
#include <ostream>
#include <chrono>


#include "version.h"
#include "misc.h"
#include "match.h"
#include "capture.h"
#include "character.h"
#include "range.h"

#include "word.h"
#include "ciword.h"
#include "and.h"
#include "or.h"

#include "repeat.h"
#include "blanks.h"

#include "not.h"

#include "optional.h"

#include "invoke.h"
#include "load-on-demand.h"

using namespace BeeFishBString;

namespace BeeFishParser
{
   

   class Parser
   {
   protected:
      BeeFishMisc::optional<bool> _result;
      Match& _match;
      size_t _charCount = 0;
      size_t _utf8Count = 0;

      UTF8Character _utf8;

   public:
      Parser(Match& match) :
         _match(match)
      {
      }
      
      virtual ~Parser()
      {

      }
      
      Match* getMatch() {
         return &_match;
      }

      unsigned long now()
      {
         return
            std::chrono::duration_cast
            <std::chrono::milliseconds>
            (
               std::chrono::system_clock
                  ::now()
                     .time_since_epoch()
            ).count();
      }

      virtual bool match(char c) {

#ifdef DEBUG
         cout << c;
#endif

         ++_charCount;

         _utf8.match(c);

         if (_utf8.result() == true) {
            // Valid utf8 character, perform match
            _match.match(this, _utf8.character());
            // Reset the utf8 character
            _utf8.reset();
            ++_utf8Count;
         }
         else if (_utf8.result() == false) {
            _utf8.reset();
            _result = false;
            return false;
         }

         _result = _match._result;
         
         if (_result == BeeFishMisc::nullopt || _result == true)
            return true;

         return false;

      }
      
      virtual BeeFishMisc::optional<bool> read(
         istream& input
      )
      {
       
#ifdef TIME
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         _result = BeeFishMisc::nullopt;
         _match._parser = this;

         int i = 0;
         while ((i = input.get()) != -1)
         {
            
            char c = i;

            DEBUG_OUT(c);

            if (!match(c))
               return false;

#ifdef TIME
            if (_charCount % 10000 == 0)
            {
               unsigned long time =
                  now() - start;
               
               cout 
                  << _charCount << " (char count)\t"
                  << Match::matchInstanceCount() << " (instances)\t" 
                  << time << " (milliseconds)\t"
                  << 10000.0 / time * 1000.0 << " (chars per second)" 
                  << endl;

               start = now();
            }
#endif
            if (_result != BeeFishMisc::nullopt) {
               break;            
            }
         }

         return _result;
      }
   
      virtual BeeFishMisc::optional<bool> read(const string& str)
      {
      
         istringstream input(str);
      
         return read(input);
      
      }
      
      virtual BeeFishMisc::optional<bool> read(const char* str) {
         return read(std::string(str));
      }

      virtual BeeFishMisc::optional<bool> read(const BeeFishBString::Data& data)
      {

         const Byte* _data = data.data();
         size_t _size = data.size();

         for (size_t i = 0; i < _size; ++i) {
            Byte byte = _data[i];
            if (!match((char)byte))
               return false;
         }

         return _result;
      
      }

      BeeFishMisc::optional<bool> result() const
      {
         return _result;
      }

   };
}

#endif