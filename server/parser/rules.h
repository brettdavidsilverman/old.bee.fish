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
#include "match-ptr.h"

using namespace std;

namespace bee::fish::parser {
 
   inline MatchPointer<And> operator and
   (
      MatchPointerBase first,
      MatchPointerBase second
   )
   {
      return new And(
         first.get(),
         second.get()
      );
      
   }
   
   inline MatchPointer<Or> operator or
   (
      MatchPointerBase first,
      MatchPointerBase second
   )
   {
      return new Or(
         first.get(),
         second.get()
      );
   }
   
   inline MatchPointer<Optional> operator ~
   (MatchPointerBase match)
   {
      return new Optional(match.get());
   }
   
   inline MatchPointer<Not> operator not
   (MatchPointerBase match)
   {
      return new Not(match.get());
   }

   inline Match* Optional2(Match* optional, Match* next)
   {
      return new Or(
         next,
         new And(optional, next->copy())
      );
      
   }
   
   

}

#endif