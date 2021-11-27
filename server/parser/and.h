#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include "../misc/optional.h"
#include "match.h"

using namespace std;

namespace BeeFishParser {

   class And : public Match {
   protected:
      vector<Match*> _inputs;
      vector<Match*>::iterator _iterator;
   public:

      template<typename ...T>
      And(T*... inputs) :
         _inputs{inputs...}
      {
         _iterator = _inputs.begin();
      }
      
      virtual ~And()
      {
         for (auto it : _inputs)
         {
            Match* match = it;
            delete match;
         }
      }

      virtual void setup() {
         _iterator = _inputs.begin();
         Match::setup();
      }     

      virtual bool match(const Char& character) {
      
         bool matched = false;
            
         if ( _iterator == _inputs.end() ) {
            fail();
            return false;
         }
            
         while ( !matched &&
                 _result == BeeFishMisc::nullopt )
         {

            Match* item = *_iterator;

            matched =
               item->matchCharacter(character);
         
            if (item->_result == true) {
            
               if ( ++_iterator == 
                    _inputs.end() ) {
                  success();
               }
               
            }
            else if (item->_result == false) {
            
               fail();
               
            }
            
         }

         return matched;
         
      }
      
   };

};

#endif
