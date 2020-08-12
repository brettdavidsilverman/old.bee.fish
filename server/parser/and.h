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
         _inputs.push_back(_and);
         return this;
      }
      
      virtual bool
      match
      (
         int character,
         optional<bool>& success
      )
      {
      
         bool matched = false;
         vector<Match*>::const_iterator 
            end = _inputs.cend();
            
         for (;;)
         {

            Match* item = *_iterator;

            optional<bool> 
               childSuccess = nullopt;
         
            matched =
               item->match(
                  character,
                  childSuccess
               );
         
            if (matched)
               Match::match(character, success);

            if (childSuccess == true) {
            
               if ( ++_iterator == end )
               {
                  success = true;
                  onsuccess();
               }
            }
            else if (childSuccess == false) {
               success = false;
               onfail();
            }
            
            if ( matched ||
                 success != nullopt )
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
