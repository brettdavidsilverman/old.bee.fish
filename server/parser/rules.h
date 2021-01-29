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

   inline Match operator and(const Match& first, const Match& second)
   {
      Match result = And(first, second);
      return result;
   }
   
   inline Match operator or(const Match& first, const Match& second)
   {
      Match result = Or(first, second);
      return result;
   }
   
   inline Match operator ~(const Match& match)
   {
      Match result = Optional(match);
      return result;
   }
   
   inline Match operator not(const Match& match)
   {
      Match result = Not(match);
      return result;
   }
   

}

#endif