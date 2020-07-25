#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <experimental/optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      Match* _item = NULL;
   
   public:

      template<typename ...T>
      Or(T*... inputs) :
         Match(inputs...)
      {
      }
      
      virtual ~Or() {
      }
   
      virtual bool
      match(int character)
      {
   
         bool matched = false;
                  
         for ( auto
              it  = _inputs.begin();
              it != _inputs.end();
            ++it )
         {
         
            Match* item = *it;
   

            if (item->success() == 
                nullopt)
            {
       
               if (item->match(character)) {
                  matched = true;
                  Match::match(character);
               }
               
               if (item->success() ==
                   true)
               {
                  _item = item;
                  success() = true;
                  break;
               }
            
            }
            
       
         }
      
         if (success() == nullopt && 
             !matched)
         {
            success() = false;
         }
      
  
         
         return matched;
      }
   
   
      virtual const Match& item() const {
         if (success() != true)
            throw
               std
               ::experimental
               ::bad_optional_access
               ("Or::item()");
         return *_item;
      }
   
      virtual const string& value() const
      {
         return item().value();
      }
   
      virtual void
      write(ostream& out) const
      {
         out << "Or(";
      
         Match::write(out);
      
         out << ")";
      }

   
   
   };


};

#endif
