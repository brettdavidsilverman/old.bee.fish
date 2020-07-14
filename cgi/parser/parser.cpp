#include "parser.h"


namespace bee::fish::parser {

ostream& operator <<
(ostream& out,
const optional<bool>& success) {
   if (success == true)
      out << "1";
   else if (success == false)
      out << "0";
   else
      out << "?";
   return out;
}

}