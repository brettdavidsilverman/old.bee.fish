#ifndef BEE_FISH_JSON__SET_H
#define BEE_FISH_JSON__SET_H
#include <ostream>
#include <vector>
#include <optional>
#include "../parser/parser.h"
#include "blank-space.h"

using namespace std;

namespace bee::fish::json {

   Match Set( const Match& openBrace,
           const Match& item,
           const Match& seperator,
           const Match& closeBrace )
   {
      const Match OpenBrace =
         openBrace and ~BlankSpace;
         
      const Match Seperator =
         ~BlankSpace and
         seperator and
         ~BlankSpace;
         
      const Match CloseBrace =
         ~BlankSpace and closeBrace;
         
      const Match set =
         OpenBrace and
         Optional(
            item and
            Repeat(Seperator and item, 0)
         ) and
         CloseBrace;
         
      return set;
   }
      


};

#endif
