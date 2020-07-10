#include <iostream>

using namespace std;

namespace Bee::Fish::Parser {

class Match;

ostream& operator << 
(
   ostream& out,
   const Match& match
);

ostream& operator <<
(ostream& out,
const vector<Match*>& items);

ostream& operator <<
(ostream& out,
const optional<bool>& success);

};