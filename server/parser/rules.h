#ifndef BEE_FISH_PARSER__RULES_H
#define BEE_FISH_PARSER__RULES_H
#include <string>
#include <vector>
#include <iostream>
#include "../misc/optional.h"
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

namespace BeeFishParser {
 
   MatchPointer<And> operator and
   (
      MatchPointerBase first,
      MatchPointerBase second
   )
   {

      And* _and =
         new And(
            first.get(),
            second.get()
         );

      MatchPointer<And> pointer(_and);

      return pointer;
   }
   
   MatchPointer<Or> operator or
   (
      MatchPointerBase first,
      MatchPointerBase second
   )
   {
      Or* _or = new Or(
         first.get(),
         second.get()
      );

      MatchPointer<Or> pointer(_or);

      return pointer;
   }
   
   MatchPointer<Optional> operator ~
   (MatchPointerBase match)
   {
      Optional* optional = new Optional(match.get());
      MatchPointer<Optional> pointer(optional);
      return pointer;
   }
   
   MatchPointer<Optional>
   _Optional(const Match& match)
   {
      Optional* optional = new Optional(match.copy());
      MatchPointer<Optional> pointer(optional);
      return pointer;
   }
   
   MatchPointer<Not> operator not
   (MatchPointerBase match)
   {
      Not* _not = new Not(match.get());
      MatchPointer<Not> pointer(_not);
      return pointer;
   }

   MatchPointer<Or> Optional2(Match* optional, Match* next)
   {
      Or* _or = new Or(
         next,
         new And(optional, next->copy())
      );

      MatchPointer<Or> pointer(_or);

      return pointer;
      
   }
   
   

}

#endif