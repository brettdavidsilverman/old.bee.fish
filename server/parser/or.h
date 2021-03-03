#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      MatchPtr _first;
      MatchPtr _second;
      
   public:

      Or(MatchPtr first, MatchPtr second) :
         _first(first),
         _second(second)
      {
      }
      
      Or(const Or& source) :
         _first(source._first->copy()),
         _second(source._second->copy())
      {
      }
      
      virtual ~Or()
      {
      }
      
      virtual bool match(Char character)
      {
   
         bool matched = false;
        
         if (_first->_result == nullopt)
         {
            matched |= _first->match(character);
            if (_first->_result == true)
            {
               success();
            }
         }
         
         if (_second->_result == nullopt)
         {
            matched |= _second->match(character);
            if (_second->_result == true)
            {
               success();
            }
         }

         if ( ( _first->_result == false &&
               _second->_result == false ) )
            fail();
            
         if (matched)
            capture(character);
            
         return matched;
         
         
      }
   
      virtual Match& item() {
         if (_first->_result == true)
            return *_first;
         else if (_second->_result == true)
            return *_second;
         else
            throw runtime_error(
               "None of the items succeeded in Or"
            );
      }

      virtual MatchPtrBase copy() const
      {
         return make_shared<Or>(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << "Or";
         
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
