#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"

using namespace std;

namespace bee::fish::parser {

   class And : public Match {
   protected:
      Match _first;
      Match _second;

   public:

      And(const Match& first, const Match& second) :
         Match("And"),
         _first(first),
         _second(second)
      {
      }
      
      And(const And& source) :
         Match(source.name()),
         _first(source._first),
         _second(source._second)
      {
      }
         
      virtual ~And()
      {
      }
     
      virtual bool
      match(int character) {
      
         bool matched = false;
         
         while ( !matched &&
                 _first.result() == nullopt )
         {
            matched |= _first.match(character);
         }
         
         while ( !matched &&
                 _first.result() == true &&
                 _second.result() == nullopt )
         {
            matched |= _second.match(character);
         }
         
         if ( _first.result() == true && 
              _second.result() == true )
            success();
         else if ( _first.result() == false || 
                   _second.result() == false )
            fail();
            
         return matched;
         
      }
      
      virtual Match* copy() const
      {
         return new And(*this);
      }
   
      virtual void write(ostream& out) const
      {
         Match::write(out);
         
         out << "("
             << _first 
             << ", " 
             << _second
             << ")";

      }

   };

};

#endif
