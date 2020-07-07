#include "parser.h"

using namespace std;

namespace Bee::Fish::Parser {

ostream& operator <<
(ostream& out, const Match& match)
{
   optional<bool> success =
      match.success();
      
   if (success == true)
      out << "Match:" << match.value();
   else if (success == false)
      out << "Failed";
   else
      out << "No Match";
   
   return out;
}

ostream& operator <<
(ostream& out,
const vector<Match*>& items)
{
   int i = 0;
   for (vector<Match*>::const_iterator
           it = items.begin();
           it != items.end();
         ++it)
   {
      const Match* item = *it;
      
      if (item->success() == true) {
         out << ++i << item->value();
      }
         
   }
   
   return out;

};

};