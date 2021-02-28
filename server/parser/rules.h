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

   inline MatchPtr operator and(const MatchPtr& first, const MatchPtr& second)
   {
      return new And(first, second);
   }
   
   inline MatchPtr operator or(const MatchPtr first, const MatchPtr second)
   {
      return new Or(first, second);
   }
   
   inline MatchPtr operator ~(const MatchPtr match)
   {
      return new Optional(match);
   }
   
   inline MatchPtr operator not(const MatchPtr match)
   {
      return new Not(match);
   }
   
   inline MatchPtr Optional2(const MatchPtr optional, const MatchPtr next)
   {
      return ((optional and next) or next);
   }
   
   

}

#endif