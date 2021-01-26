#ifndef BEE_FISH_PARSER__SET_H
#define BEE_FISH_PARSER__SET_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"
#include "rules.h"

using namespace std;

namespace bee::fish::parser {

   class Set : public And {
   protected:

   public:

      Set( const Match& openBrace,
           const Match& item,
           const Match& seperator,
           const Match& closeBrace ) :
         And(
            openBrace and
            ~(
               item and
               ~Repeat(
                  seperator and item
               )
            ) and
            closeBrace
         )
      {
      }
      
      Set(const Set& source) :
         And(source)
      {
      }
         
      virtual ~Set()
      {
      }
      
      virtual string name()
      {
         return "Set";
      }
      
      virtual Match* copy() const
      {
         return new Set(*this);
      }
   
   };

};

#endif
