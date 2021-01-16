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

   virtual bool match
   (
      int character,
      optional<bool>& success
   )
   {
      
      optional<bool> childSuccess = nullopt;
      
      bool matched =
         _match->match(character, childSuccess);
         
      if (!matched)
         Match::match(character, success);
     
      
      if (childSuccess == false)
         success = true;
      else if (childSuccess == true)
         success = false;
      else if (character == Match::EndOfFile) {
         success = true;
      }

      if (success == true)
         onsuccess();
         
      return !matched;
      
   }
   
   virtual string name()
   {
      return "Not";
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
