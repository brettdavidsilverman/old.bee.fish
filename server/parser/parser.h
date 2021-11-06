#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include <sstream>
#include <ostream>
#include <chrono>

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

using namespace bee::fish::b_string;

namespace bee::fish::parser
{
   

   class Parser
   {
   protected:
      Match& _match;
      size_t _charCount = 0;
      
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
      
      virtual std::optional<bool> read(
         istream& input
      )
      {
       
#ifdef TIME
         unsigned long readCount = 0;
         
         cout << "Chars" << "\t" << "Matches" << "\t" << "Time" << endl;
         unsigned long start = now();
#endif

         
         char character = 0;
         while (!input.eof())
         {

            input.get(character);

            ++_charCount;
#ifdef DEBUG
            cerr << character;
#endif
            _match.match(character);

#ifdef TIME
            if (++readCount % 1000 == 0)
            {
               unsigned long time =
                  now() - start;
               
               cout << readCount << "\t" << Match::_matchInstanceCount << "\t" << time << endl;
               start = now();
            }
#endif
            if (result() != std::nullopt)
               break;            
         }
         
         return result();
      }
   
      virtual std::optional<bool> read(const string& str)
      {
      
         istringstream input(str);
      
         return read(input);
      
      }
      
      std::optional<bool> result() const
      {
         return _match.result();
      }

   };
}

#endif