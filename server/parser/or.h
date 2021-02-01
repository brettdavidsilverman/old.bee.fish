#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      Match _first;
      Match _second;
      
   public:

      Or(const Match& first, const Match& second) :
         _first(first),
         _second(second)
      {
      }
      
      Or(const Or& source) :
         _first(source._first),
         _second(source._second)
      {
      }
      
      virtual ~Or()
      {
      }
      
      virtual bool match(int character)
      {
   
         bool matched = false;
        
         if (_first.result() == nullopt)
         {
            matched |= _first.match(character);
            if (_first.result() == true)
            {
               success();
               return matched;
            }
         }
         
         if (_second.result() == nullopt)
         {
            matched |= _second.match(character);
            if (_second.result() == true)
            {
               success();
               return matched;
            }
         }

         if ( ( _first.result() == false &&
               _second.result() == false ) )
            fail();
            
         return matched;
         
         
      }
   
      virtual Match& item() {
         if (_first.result() == true)
            return _first;
         else if (_second.result() == true)
            return _second;
         else
            throw runtime_error(
               "None of the items succeeded in Or"
            );
      }

      virtual string name() const
      {
         return "Or";
      }
      
      virtual Match* copy() const
      {
         return new Or(*this);
      }
      
      virtual void write(ostream& out) const
      {
         Match::write(out);
         
         out << "("
             << _first
             << " or "
             << _second
             << ")";
      }
   };


};

#endif
