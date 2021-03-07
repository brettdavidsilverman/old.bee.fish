#ifndef BEE_FISH_PARSER__RULES_H
#define BEE_FISH_PARSER__RULES_H
#include <string>
#include <vector>
#include <iostream>
#include <optional>
#include "version.h"
#include "match.h"

#include "character.h"
#include "range.h"
#include "word.h"
#include "ciword.h"

#include "or.h"
#include "and.h"
#include "not.h"
#include "optional.h"

#include "repeat.h"
#include "load-on-demand.h"


using namespace std;

namespace bee::fish::parser {

   inline MatchPtr operator and(MatchPtr first, MatchPtr second)
   {
      MatchPtr pointer =
         make_shared<And>(first, second);
      return pointer;
   }
   
   inline MatchPtr operator or(MatchPtr first, MatchPtr second)
   {
      MatchPtr pointer =
         make_shared<Or>(first, second);
      return pointer;
   }
   
   inline MatchPtr operator ~(MatchPtr match)
   {
      MatchPtr pointer =
         make_shared<Optional>(match);
      return pointer;
   }
   
   inline MatchPtr operator not(MatchPtr match)
   {
      MatchPtr pointer =
         make_shared<Not>(match);
      return pointer;
   }
   
   inline MatchPtr Optional2(MatchPtr optional, const MatchPtr next)
   {
      MatchPtr pointer = (next or (optional and next));
      return pointer;
   }
   
   

}

#endif