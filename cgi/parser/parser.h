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


using namespace std;

namespace bee::fish::parser {

   And operator and (
      const Match& and1,
      const Match& and2);

   Or operator or (
      const Match& or1,
      const Match& or2);

   Not operator not (const Match& _not);

   void test();

}

#endif