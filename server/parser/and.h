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
      
         bool matched = false;
         
         for (;;)
         {
         
            if (*_iterator == NULL)
            {
               _iterator = 
                  next(_iterator);
                  
               if ( _iterator ==
                    _inputs.end() )
               {
                  success = true;
                  onsuccess();
                  break;
               }
            }
         
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
            
               _iterator = next(_iterator);
            
               if ( _iterator ==
                    _inputs.end() )
               {
                  success = true;
                  onsuccess();
                  break;
               }
            }
            else if (childSuccess == false) {
               success = false;
               onfail();
               break;
            }
            
            if ( matched ||
                 success != nullopt )
               break;
         }

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
