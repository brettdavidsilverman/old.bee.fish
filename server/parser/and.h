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
      Match* _first;
      Match* _second;
      bool _isFirst = true;
   public:

      And(const Match& first, const Match& second) :
         _first(first.copy()),
         _second(second.copy())
      {
      }
      
      And(const And& source) :
         _first(source._first->copy()),
         _second(source._second->copy())
      {
      }
         
      virtual ~And()
      {
         delete _first;
         delete _second;
      }
      
      friend And operator and(const Match& first, const Match& second);

      virtual bool
      match(int character) {
      
         bool matched = false;
            
         for (;;) {

            Match* item =
               _isFirst ?
                  _first :
                  _second ;

            matched =
               item->match(character);
         
            if (matched)
               Match::match(character);

            if (item->result() == true) {
            
               if ( !_isFirst ) {
                  success();
               }
               else
                  _isFirst = !_isFirst;
            }
            else if (item->result() == false) {
            
               fail();
               
            }
            
            if ( matched ||
                 result() != nullopt )
               break;
         }

         return matched;
      }
      
      virtual string name()
      {
         return "And";
      }
      
      virtual Match* copy() const
      {
         return new And(*this);
      }
   
   };

};

#endif
