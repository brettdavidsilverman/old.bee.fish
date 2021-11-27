#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include "../misc/optional.h"
#include "match.h"

namespace BeeFishParser {

   class Or : public Match {
   public:
      vector<Match*> _inputs;
      Match* _item = nullptr;
      size_t _index = 0;
      
   public:

      template<typename ...T>
      Or(T*... inputs) :
         _inputs{inputs...}
      {
      }
      
      virtual ~Or()
      {
         for (auto it : _inputs) {
            Match* match = it;
            delete match;
         }
      }
      
      virtual bool match(const Char& character)
      {
   
         bool matched = false;
         _index = 0;
         
         for ( auto
                 it  = _inputs.begin();
                 it != _inputs.end();
                ++_index, ++it
             )
         {
         
            Match* item = *it;
            
            if ( item->_result != BeeFishMisc::nullopt )
               continue;

            if ( item->matchCharacter(character) )
            {
               matched = true;
            }
            
            if ( item->_result == true )
            {
               _item = item;
               break;
            }

       
         }
       
         if (_item)
            success();
         else if ( _result == BeeFishMisc::nullopt && 
                   !matched )
            fail();
         
         return matched;
         
      }
   
      const BString& value() const
      {
         return _item->value();
      }
      
      virtual Match& item()
      {
         return *_item;
      }
      
   };


};

#endif
