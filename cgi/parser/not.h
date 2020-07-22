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
         set_success(true);
      else if (_match->success() == true)
         set_success(false);
      else if (character == Match::eof) {
         set_success(true);
      }

      return !matched;
      
   }
   
   virtual void write(ostream& out) const
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
