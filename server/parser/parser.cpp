#include "parser.h"



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

namespace bee::fish::parser {

   const string Optional::_defaultValue = "";


}