#ifndef BEE_FISH_PARSER__SET_H
#define BEE_FISH_PARSER__SET_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"
#include "rules.h"

using namespace std;

namespace bee::fish::parser {

   Match Set( const Match& openBrace,
           const Match& item,
           const Match& seperator,
           const Match& closeBrace )
   {
      return
         openBrace and
         ~(
            item and
            ~Repeat(
               seperator and item
            )
         ) and
         closeBrace;
   }
      


};

#endif
