#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      Match* _item = NULL;
      size_t _index = 0;
      
   public:

      template<typename ...T>
      Or(T*... inputs) :
         Match(inputs...)
      {
      }
      
      virtual ~Or()
      {
      }
  
      virtual bool match
      (
         int character,
         optional<bool>& success
      )
      {
   
         bool matched = false;
         optional<bool> childSuccess;
         _index = 0;
         auto end = _inputs.end();
         
         for ( auto
                 it  = _inputs.begin();
                 it != end;
                ++_index, ++it
             )
         {
         
            Match* item = *it;
            
            if (!item)
               continue;
               
            childSuccess = nullopt;
            if (item->match(
                   character,
                   childSuccess)
                )
            {
               matched = true;
            }
            
            if (childSuccess == true)
            {
               _item = item;
               success = true;
               onsuccess();
               return matched;
            }
            else if (
               !matched ||
               (childSuccess ==
                         false)
            )
            {
               delete item;
               *it = NULL;
            }
            
       
         }
      
         if (success == nullopt && 
             !matched)
         {
            success = false;
            onfail();
         }
         else if (matched)
            Match::match(character, success);
        
         
         return matched;
      }
   
   
      virtual Match& item() {
         return *_item;
      }
   
      virtual string& value()
      {
         return item().value();
      }
      
      virtual size_t index()
      {
         return _index;
      }
      
      virtual string name()
      {
         return "Or";
      }
      
   
   };


};

#endif
