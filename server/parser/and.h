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
      MatchPtr _first;
      MatchPtr _second;

   public:

      And(const MatchPtr& first, const MatchPtr& second) :
         _first(first),
         _second(second)
      {
      }
      
      And(const And& source) :
         _first(source._first->copy()),
         _second(source._second->copy())
      {
      }

      virtual ~And()
      {
      }
     
      virtual bool
      match(Char character) {
      
         bool matched = false;
         
         if ( !matched &&
                 _first->_result == nullopt )
         {
            matched |= _first->match(character);
         }
         
         if ( !matched &&
              _first->_result == true &&
              _second->_result == nullopt )
         {
            matched |= _second->match(character);
         }
         
         if ( _first->_result == true && 
              _second->_result == true )
            success();
         else if ( _first->_result == false || 
                   _second->_result == false )
            fail();
            
         if (matched)
            capture(character);
            
         return matched;
         
      }
      
      virtual MatchPtr copy() const
      {
         return new And(*this);
      }
   
      virtual void write(ostream& out) const
      {
         out << "And";
         
         writeResult(out);
         
         out << "("
             << *_first 
             << ", " 
             << *_second
             << ")";

      }

   };

};

#endif
