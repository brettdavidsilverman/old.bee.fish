#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <experimental/optional>
#include "match.h"

using namespace std;

namespace bee::fish::parser {

   class And : public Match {
   protected:
   
      vector<Match*>::iterator
         _iterator;

   public:

      template<typename ...T>
      And(T*... inputs) :
         Match(inputs...)
      {
         _iterator = _inputs.begin();
      }
      
      virtual ~And() {
      }
   
      And* operator and(Match* _and)
      {
         inputs().push_back(_and);
         return this;
      }
      
      virtual bool
      match(int character)
      {
      
 
         Match* item = (*_iterator);

         bool matched =
            item->match(character);
         
         if (matched)
            Match::match(character);

         if (item->success() == true) {
            if (++_iterator ==
                  _inputs.end())
            {
               setSuccess(true);
            }
         }
         else if (item->success() == false)
            setSuccess(false);
         
         if (!matched && success() == nullopt)
            matched = And::match(character);
            
         return matched;
      }
   
      virtual void write(ostream& out)
      {
         out << "And(";
      
         Match::write(out);
      
         out << ")";
      }
   
   
   };

};

#endif
