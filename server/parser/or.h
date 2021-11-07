#ifndef BEE_FISH_PARSER__OR_H
#define BEE_FISH_PARSER__OR_H
#include <vector>
#include "../misc/optional.h"
#include "match.h"

namespace BeeFishParser {

   class Or : public Match {
   public:
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

            if ( item->match(character) )
            {
               matched = true;
            }
            
            if ( item->_result == true )
            {
               _item = item;
               break;
            }

       
         }
       
         if (matched)
            capture(character);
        
         if (_item)
            success();
         else if ( _result == BeeFishMisc::nullopt && 
                   !matched )
            fail();
         
         return matched;
         
      }
   
      const BeeFishBString::BStringBase& value() const
      {
         return _item->value();
      }
      
      virtual Match& item()
      {
         return *_item;
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
      
         std::string tabs = Match::tabs(tabIndex);
         
         out << tabs << "Or";
         
         writeResult(out);
         out << endl;
         out << tabs << "(" << endl;
         writeInputs(out, tabIndex + 1);
         out << tabs << ")";
      }
   };


};

#endif
