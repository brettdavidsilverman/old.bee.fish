#ifndef BEE_FISH_JSON__NULL_H
#define BEE_FISH_JSON__NULL_H

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{
   const Label Null = Label("Null", Word("null"));
   
}

#endif