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

   inline And operator and(const Match& first, const Match& second)
   {
      And result = And(first, second);
      return result;
   }
   
   inline Or operator or(const Match& first, const Match& second)
   {
      Or result = Or(first, second);
      return result;
   }
   
   inline Optional operator ~(const Match& match)
   {
      Optional result = Optional(match);
      return result;
   }
   
   inline Not operator not(const Match& match)
   {
      Not result = Not(match);
      return result;
   }
   

}

#endif