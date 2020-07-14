#ifndef BEE_FISH_PARSER__STREAM_H
#define BEE_FISH_PARSER__STREAM_H

#include <iostream>
#include "match.h"

using namespace std;


namespace bee::fish::parser {

ostream& operator <<
(ostream& out,
const optional<bool>& success);

#endif