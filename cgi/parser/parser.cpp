#include "parser.h"


namespace bee::fish::parser {

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

ostream& operator <<
(ostream& out, const Match& match)
{

   match.write(out);
   out << match.success() << ": ";
   out << "<" << match.value() << ">";
   
   return out;
}

ostream& operator <<
(ostream& out,
const vector<Match*>& items)
{
   int i = 0;
   for (auto
           it = items.cbegin();
           it != items.cend();
         ++it, ++i)
   {
      const Match* item = *it;
      
      out << "[" << i << "]"
          << *item;
         
   }
   
   return out;

}



}