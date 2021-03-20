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
         _iterator = _inputs.end();
      }
      
      And(const And& source) :
         Match(source)
      {
         _iterator = _inputs.end();
      }

      virtual ~And()
      {
      }
     
      virtual bool
      match(Char character) {
      
         bool matched = false;
            
         if ( _iterator == _inputs.end() )
            _iterator = _inputs.begin();
            
         while ( !matched &&
                 _result == nullopt )
         {

            Match* item = *_iterator;

            matched =
               item->match(character);
         
            if (matched)
               capture(character);

            if (item->result() == true) {
            
               if ( ++_iterator == 
                    _inputs.end() ) {
                  success();
               }
               
            }
            else if (item->result() == false) {
            
               fail();
               
            }
            
         }

         return matched;
         
      }
      
      virtual Match* copy() const
      {
         return new And(*this);
      }
   
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         BString tabs = Match::tabs(tabIndex);
         
         out << tabs << "And";
         
         writeResult(out);
         
         out << endl;
         
         out << tabs << "(" << endl;
         writeInputs(out, tabIndex + 1);
         out << tabs << ")";

      }

   };

};

#endif
