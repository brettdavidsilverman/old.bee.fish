#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H

#include <string>
#include <vector>
#include <iostream>
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

#include "not.h"

#include "optional.h"

#include "invoke.h"
#include "load-on-demand.h"

/*


#include "match-ptr.h"

#include "base64.h"
*/

using namespace BeeFishBString;

namespace BeeFishParser
{
   

   class Parser
   {
   protected:
      BeeFishMisc::optional<bool> _result = BeeFishMisc::nullopt;
      Match& _match;
      size_t _charCount = 0;
      UTF8Character _utf8;

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

         int i =0;
         while ((i = input.get()) != -1)
         {
            
            char c = i;

#ifdef DEBUG
            cout << c;
#endif
            ++_charCount;

            bool matched = _utf8.match(c);


            if (matched) {
               // Valid byte sequence, check if full character
               if (_utf8.result() == true) {
                  // Valid utf8 character, perform match
                  matched = _match.matchCharacter(_utf8.character());
                  // Reset the utf8 character
                  _utf8.reset();
               }
               else if (_utf8.result() == false) {
                  _result = false;
                  break;
               }
            }
            
            /*
            if (!matched) {
               // Invalid sequence,
               _result = false;
               return false;
            }
            */

#ifdef TIME
            if (++readCount % 1000 == 0)
            {
               unsigned long time =
                  now() - start;
               
               cout << readCount << "\t" << Match::_matchInstanceCount << "\t" << time << endl;
               start = now();
            }
#endif
            if (_match._result != BeeFishMisc::nullopt) {
               break;            
            }
         }

         if (_result == BeeFishMisc::nullopt)
            _result = _match._result;
         
         return _result;
      }
   
      virtual BeeFishMisc::optional<bool> read(const string& str)
      {
      
         istringstream input(str);
      
         return read(input);
      
      }
      
      BeeFishMisc::optional<bool> result() const
      {
         return _result;
      }

   };
}

#endif