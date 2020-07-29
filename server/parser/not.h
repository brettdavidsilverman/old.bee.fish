#ifndef BEE_FISH_PARSER__NOT_H
#define BEE_FISH_PARSER__NOT_H

#include "match.h"

namespace bee::fish::parser {

class Not : public Match{
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

   virtual bool
   match(int character)
   {
      
      bool matched =
         _match->match(character);
         
      if (!matched)
         Match::match(character);
     
      
      if (_match->success() == false)
         setSuccess(true);
      else if (_match->success() == true)
         setSuccess(false);
      else if (character == Match::endOfFile) {
         setSuccess(true);
      }

      return !matched;
      
   }
   
   virtual void write(ostream& out) 
   {
      out << "Not(";
      Match::write(out);
      out << ":" << *_match;
      out << ")";
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
