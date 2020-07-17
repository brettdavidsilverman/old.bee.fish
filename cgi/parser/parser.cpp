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

   const string Optional::default_value = "";

   And operator and (
      const Match& and1,
      const Match& and2)
   {
      return And(and1.copy(), and2.copy());
   }

   Or operator or (
      const Match& or1,
      const Match& or2)
   {
      return Or(or1.copy(), or2.copy());
   }

   Not operator not (const Match& _not)
   {
      return Not(_not.copy());
   }

}