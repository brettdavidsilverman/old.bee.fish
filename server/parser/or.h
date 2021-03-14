#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include <optional>
#include "match.h"

namespace bee::fish::parser {

   class Or : public Match {
   protected:
      Match* _item = nullptr;
      size_t _index = 0;
      
   public:

      template<typename ...T>
      Or(T*... inputs) :
         Match(inputs...)
      {
      }
      
      Or(const Or& source) :
         Match(source)
      {
      }
      
      virtual ~Or()
      {
      }
      
      virtual bool match(Char character)
      {
   
         bool matched = false;
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

            if (item->match(character))
            {
               matched = true;
            }
            
            if (item->result() == true)
            {
               _item = item;
               break;
            }
            else if (
               !matched ||
               (item->result() == false)
            )
            {
               delete item;
               *it = NULL;
            }
            
       
         }
       
         if (matched)
            capture(character);
        
         if (_item)
            success();
         else if ( _result== nullopt && 
                   !matched )
            fail();
         
         return matched;
         
      }
   
      virtual Match* item() const
      {
         return _item;
      }

      virtual Match* copy() const
      {
         return new Or(*this);
      }
      
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
      
         BString tabs = Match::tabs(tabIndex);
         
         out << tabs << "Or";
         
         writeResult(out);
         out << endl;
         out << tabs << "(";
         writeInputs(out, tabIndex + 1);
         out << tabs << ")";
      }
   };


};

#endif
