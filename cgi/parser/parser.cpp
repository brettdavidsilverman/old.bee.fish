#include "parser.h"

using namespace Bee::Fish::Parser;

ostream& operator <<
(ostream& out, const Match& match)
{
   if (match.getSuccess())
      out << "Match: " << match.getValue();
   else if (match.getFailed())
      out << "Failed";
   else
      out << "No Match";
      
   return out;
}
