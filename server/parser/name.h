#ifndef BEE_FISH_PARSER__NAME_H
#define BEE_FISH_PARSER__NAME_H
#include <ostream>
#include <vector>
#include <optional>
#include "match.h"

using namespace std;

namespace bee::fish::parser {

   class Name : public Match
   {
   
   public:
      Name(const string& name, const Match& item) :
         Match(item)
      {
         _name = name;
         _isNamed = true;
      }
   
      Name(const Name& source) :
         Match(source)
      {
         _name = source._name;
         _isNamed = source._isNamed;
      }
   
      virtual Match* copy() const
      {
         return new Name(*this);
      }
   };


};

#endif
