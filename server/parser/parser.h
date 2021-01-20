#ifndef BEE_FISH_PARSER_H
#define BEE_FISH_PARSER_H
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
#include "and.h"
#include "or.h"
#include "not.h"
#include "repeat.h"
#include "optional.h"
#include "load-on-demand.h"

using namespace std;

namespace bee::fish::parser {

   bool test();
   
   inline And& operator and(const Match& first, const Match& second)
   {
      And* result = new And(first.copy(), second.copy());
      return *result;
   }
   
   inline And& operator and(And& first, const Match& second)
   {
      first._inputs.push_back(second.copy());
      
      return first;
   }
   
   inline Match& operator not(const Match& match)
   {
      Not* result = new Not(match.copy());
      return *result;
   }
   
   inline Or& operator or(const Match& first, const Match& second)
   {
      Or* result = new Or(first.copy(), second.copy());
      return *result;
   }
   
   inline Or& operator or(Or& first, const Match& second)
   {
      first._inputs.push_back(second.copy());
      
      return first;
   }
  
}

#endif