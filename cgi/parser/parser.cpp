#include "parser.h"

using namespace std;

namespace Bee::Fish::Parser {

ostream& operator <<
(ostream& out, const Match& match)
{

   out << match.success() << ": ";
   out << "<" << match.value() << ">";
   
   return out;
}

ostream& operator <<
(ostream& out,
const vector<Match>& items)
{
   int i = 0;
   for (vector<Match>::const_iterator
           it = items.begin();
           it != items.end();
         ++it, ++i)
   {
      const Match& item = *it;
      
      if (item.success() == true) {
         out << i << item.value();
      }
         
   }
   
   return out;

};

ostream& operator <<
(ostream& out,
const optional<bool>& success) {
   if (success == true)
      out << "true";
   else if (success == false)
      out << "false";
   else
      out << "?";
   return out;
}


};