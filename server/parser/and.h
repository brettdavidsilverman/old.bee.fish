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
      match
      (
         int character,
         optional<bool>& success
      )
      {
      
 
         Match* item = (*_iterator);

         optional<bool> childSuccess = nullopt;
         
         bool matched =
            item->match(
               character,
               childSuccess
            );
         
         if (matched)
            Match::match(character, success);

         if (childSuccess == true) {
            if (++_iterator ==
                  _inputs.end())
            {
               success = true;
               onsuccess();
               return matched;
            }
         }
         else if (childSuccess == false)
            success = false;
        
         if (!matched && success == nullopt)
            matched = And::match(character, success);
         

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
