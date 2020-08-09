#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <experimental/optional>
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
      /*
         if (_item)
            delete _item;
      */
      }
  
      virtual bool match
      (
         int character,
         optional<bool>& success
      )
      {
   
         bool matched = false;
         optional<bool> childSuccess;
         
         auto end = _inputs.end();
         
         _index = 0;
         
         for ( auto
                 it  = _inputs.begin();
                 it != end;
               ++it, ++_index )
         {
         
            Match* item = *it;

            if (item == NULL)
            {
               continue;
            }
           
            childSuccess = nullopt;
            if (item->match(
                   character,
                   childSuccess)
                )
            {
               matched = true;
               Match::match(character, success);
            }
            
            if (childSuccess == true)
            {
               _item = item;
               /*
               for (auto 
                    it2 = _inputs.begin();
                    it2 != end;
                    ++it2)
               {
                  Match* child = *it2;
                  if (child && child != _item)
                  {
                     cout << 3.5 << endl;
                     cout << "\t" << child;
                     delete child;
                     *it2 = NULL;
                  }
               }
               */
               success = true;
               onsuccess();
               return matched;
            }
            else if ( (childSuccess ==
                         false) )
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
