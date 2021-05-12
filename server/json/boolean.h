#ifndef BEE_FISH_JSON__BOOLEAN_H
#define BEE_FISH_JSON__BOOLEAN_H

#include <map>
#include <iomanip>
#include <optional>

#include "../parser/parser.h"

using namespace bee::fish::parser;

namespace bee::fish::json
{

   class _Boolean : public Or
   {
   public:

      _Boolean() : Or(
         new Word("true"),
         new Word("false")
      )
      {
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex)
             << "Boolean";
         writeResult(out);
         out << endl
             << tabs(tabIndex)
             << "("
             << endl;
         writeInputs(out, tabIndex + 1);
         out << endl
             << tabs(tabIndex)
             << ")";
      }
   };
}

#endif