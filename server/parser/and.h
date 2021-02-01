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
      bool _isFirst = true;
   public:

      And(const Match& first, const Match& second) :
         _first(first),
         _second(second)
      {
      }
      
      And(const And& source) :
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
         
         if ( _first.result() == nullopt )
         {
            matched |= _first.match(character);
         }
         
         if ( !matched &&
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
      
      virtual string name() const
      {
         return "And";
      }
   
      virtual void write(ostream& out) const
      {
         Match::write(out);
         
         out << "("
             << _first 
             << " and " 
             << _second
             << ")";

      }

   };

};

#endif
