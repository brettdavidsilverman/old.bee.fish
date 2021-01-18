#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <optional>
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
         _inputs.push_back(_and);
         return this;
      }
      
      virtual bool
      match(int character) {
      
         bool matched = false;
         vector<Match*>::const_iterator 
            end = _inputs.cend();
            
         for (;;) {

            Match* item = *_iterator;

            matched =
               item->match(character);
         
            if (matched)
               Match::match(character);

            if (item->result() == true) {
            
               if ( ++_iterator == end ) {
                  success();
               }
               
            }
            else if (item->result() == false) {
            
               fail();
               
            }
            
            if ( matched ||
                 result() != nullopt )
               break;
         }

         return matched;
      }
      
      virtual string name()
      {
         return "And";
      }
   
   
   };

};

#endif
