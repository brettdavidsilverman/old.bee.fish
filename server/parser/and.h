#ifndef BEE_FISH_PARSER__AND_H
#define BEE_FISH_PARSER__AND_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"
#include "or.h"

using namespace std;

namespace bee::fish::parser {

   class And;
   
   class And : public Match {
   protected:
   
      vector<Match*>::iterator
         _iterator;

   public:

      template<typename ...T>
      And(T*... inputs) :
         Match(inputs...)
      {
         _iterator = _inputs.end();
      }
      
      virtual ~And() {
      }
      
      
      friend And& operator and(And& first, Match& second);
      /*
      And& operator and(Match& next)
      {
         _inputs.push_back(next.copy());
         return *this;
      }
      
      Or* operator or(Match* _or)
      {
         return new Or(this, _or);
      }
      */
      
      virtual bool
      match(int character) {
      
         bool matched = false;
         vector<Match*>::iterator 
            end = _inputs.end();
            
         if (_iterator == end)
            _iterator = _inputs.begin();
            
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
   
      And(const And& source) :
         Match(source)
      {
			     _iterator = _inputs.end();
      }
			   
      virtual Match* copy() const
      {
         return new And(*this);
      }
   };

};

#endif
