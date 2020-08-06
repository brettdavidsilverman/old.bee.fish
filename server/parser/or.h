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
      
      virtual ~Or() {
         if (_item)
         {
            delete _item;
            _item = NULL;
         }
      }
   
      virtual bool match
      (
         int character,
         optional<bool>& success
      )
      {
   
         bool matched = false;
         optional<bool> childSuccess;
         
         for ( auto
              it  = _inputs.begin();
              it != _inputs.end();
              ++_index )
         {
         
            Match* item = *it;
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
               for (auto 
                    it2 = _inputs.begin();
                    it2 != _inputs.end();
                    ++it2)
               {
                  if (it2 != it)
                     delete *it2;
               }
               _inputs.clear();
               success = true;
               onsuccess();
               return matched;
            }
            else if ( childSuccess ==
                      false )
            {
               delete item;
               it = _inputs.erase(it);
            }
            else
               ++it;
            
       
         }
      
         if (success == nullopt && 
             !matched)
         {
            success = false;
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
   
      virtual void
      write(ostream& out)
      {
         out << "Or(";
      
         Match::write(out);
      
         out << ")";
      }

   
   
   };


};

#endif
