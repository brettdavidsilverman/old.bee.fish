#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include "../misc/optional.h"
#include <sstream>
#include <ostream>
#include <chrono>

#if defined(DEBUG) && defined(ESP32)
extern "C" {
#include <esp32/spiram.h>
#include <esp32/himem.h>
}
#endif

#include "version.h"
#include "misc.h"
#include "match.h"

#include "character.h"

#include "range.h"
#include "word.h"
#include "ciword.h"


#include "and.h"
#include "or.h"
#include "not.h"
#include "optional.h"

#include "repeat.h"
#include "load-on-demand.h"

#include "capture.h"
#include "label.h"
#include "invoke.h"

#include "match-ptr.h"
#include "rules.h"

#include "base64.h"

using namespace BeeFishBString;

namespace BeeFishParser
{
   

   class Parser
   {
   protected:
      Match& _match;
      size_t _charCount = 0;
      UTF8Character _character;

   public:
      Parser(Match& match) :
         _match(match)
      {
      }
      
      virtual ~Parser()
      {
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
      
      virtual BeeFishMisc::optional<bool> read(
         istream& input
      )
      {
       
#ifdef TIME
         unsigned long readCount = 0;
         
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         while (true)
         {
            int i = input.get();

            if (i == -1)
               break;

            char c = i;

            ++_charCount;
#ifdef DEBUG
#ifdef ESP32
            cout << "{" << c << "}:";
            cout << "Heap:  " << (int)((float)(ESP.getHeapSize() - ESP.getFreeHeap()) / (float)ESP.getHeapSize() * 100.0);
            cout << ", PSRAM: " << (int)((float)(ESP.getPsramSize() - ESP.getFreePsram()) / (float)ESP.getPsramSize() * 100.0);
            //cout << endl;
#else
            cout << c;
#endif

#endif
            bool matched = _character.match(c);

            if (matched) {
               // Valid byte sequence, check if full character
               if (_character.result() == true) {
                  // Valid utf8 character, perform match
                  matched = _match.match(_character.character());
                  // Reset the utf8 character
                  _character.reset();
               }
               else if (_character.result() == false) {
                  _match._result = false;
                  return false;
               }
            }
            
            if (!matched) {
               // Invalid sequence,
               _match._result = false;
               return false;
            }

#ifdef TIME
            if (++readCount % 1000 == 0)
            {
               unsigned long time =
                  now() - start;
               
               cout << readCount << "\t" << Match::_matchInstanceCount << "\t" << time << endl;
               start = now();
            }
#endif
            if (result() != BeeFishMisc::nullopt)
               break;            
         }
         
         return result();
      }
   
      virtual BeeFishMisc::optional<bool> read(const string& str)
      {
      
         istringstream input(str);
      
         return read(input);
      
      }
      
      BeeFishMisc::optional<bool> result() const
      {
         return _match.result();
      }

   };
}

#endif