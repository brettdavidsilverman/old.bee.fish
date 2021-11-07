#ifndef BEE_FISH_JSON__BOOLEAN_H
#define BEE_FISH_JSON__BOOLEAN_H

#include <map>
#include <iomanip>
#include "../misc/optional.h"

#include "../parser/parser.h"

using namespace BeeFishParser;

namespace BeeFishJSON
{

   class _Boolean : public Match
   {
   public:

      Word* _true;
      Word* _false;
      
      _Boolean()
      {
      }
      
      _Boolean(const _Boolean& source) :
         Match(source)
      {
      }
      
      virtual void setup()
      {
         _true = new Word("true");
         _false = new Word("false");
         _match = new Capture(
            new Or(_true, _false)
         );
         
         _setup = true;
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
      
      virtual Match* copy() const
      {
         return new _Boolean(*this);
      }
      
      
   };
}

#endif