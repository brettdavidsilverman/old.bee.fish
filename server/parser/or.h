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
      }
   
      virtual bool
      match(int character)
      {
   
         bool matched = false;
                  
         for ( auto
              it  = _inputs.begin();
              it != _inputs.end();
            ++it, _index++ )
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
                  setSuccess(true);
                  break;
               }
            
            }
            
       
         }
      
         if (success() == nullopt && 
             !matched)
         {
            setSuccess(false);
         }
      
  
         
         return matched;
      }
   
   
      virtual Match& item() {
         if (success() != true)
            throw
               std
               ::experimental
               ::bad_optional_access
               ("Or::item()");
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
