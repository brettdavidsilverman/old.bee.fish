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
   protected:
      string _name;
   public:
   
      Name(
         const string& name,
         const Match& item
      ) :
         Match(item)
      {
         _name = name;
      }
      
      Name(const Name& source) :
         Match(source),
         _name(source._name)
      {
      }
      
      virtual Match* copy() const
      {
         return new Name(*this);
      }
      
      virtual void write(ostream& out) const
      {
         out << _name;
         writeResult(out);
         out << "()";
      }
      
      virtual bool isLabeled() const
      {
         return true;
      }
      
   };


};

#endif
