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
#include "json.h"

using namespace std;

namespace bee::fish::parser {

   bool test();

}

#endif