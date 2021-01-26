#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

   class Not : public Match{
   protected:
      Match* _match;
   public:

      Not(Match* match)
         : Match()
      {
         _match = match;
      }
   

      virtual ~Not() {
         delete _match;
      }

      virtual bool match(int character)
      {
      
         bool matched =
            _match->match(character);
         
         if (!matched)
            Match::match(character);
     
      
         if (_match->result() == false)
            success();
         else if (_match->result() == true)
            fail();
         else if (character == Match::EndOfFile) {
            success();
         }
         
         return !matched;
      
      }
   
      virtual string name()
      {
         return "Not";
      }
   
      Not(const Not& source) :
         Match(source)
      {
         _match = source._match->copy();
      }
			   
      virtual Match* copy() const
      {
         return new Not(*this);
      }
   
   };

   template<class T>
   class _Not : public Not {
   public:
      _Not() : Not( new T() ) {
      }
   };



};

#endif
