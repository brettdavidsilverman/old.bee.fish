#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      Match* _first;
      Match* _second;
      bool _failed[2] = {false, false};
      unsigned int _index;
      
   public:

      Or(const Match& first, const Match& second) :
         _first(first.copy()),
         _second(second.copy())
      {
      }
      
      Or(const Or& source) :
         _first(source._first->copy()),
         _second(source._second->copy())
      {
      }
      
      virtual ~Or()
      {
         delete _first;
         delete _second;
      }
      
      friend Or operator or(Match& first, const Match& second);

      virtual bool match(int character)
      {
   
         bool matched = false;
         for ( _index = 0;
               _index < 2;
               ++_index
             )
         {
         
            if (_failed[_index])
               continue;
               
            Match* item = 
               _index == 0 ?
                  _first :
                  _second;
            
            if (item->result() == false)
               continue;

            if (item->match(character))
            {
               matched = true;
            }
            
            if (item->result() == true)
            {
               success();
               return matched;
            }
            else if (
               !matched ||
               (item->result() == false)
            )
            {
               _failed[_index] = true;
            }
            
       
         }
      
         if (result() == nullopt && 
             !matched)
         {
            fail();
         }
         else if (matched)
            Match::match(character);
        
         
         return matched;
      }
   
   
      virtual Match& item() {
         return _index == 0 ?
            *_first :
            *_second;
      }

      virtual size_t index()
      {
         return _index;
      }
      
      virtual string name()
      {
         return "Or";
      }
      
      virtual Match* copy() const
      {
         return new Or(*this);
      }
   };


};

#endif
