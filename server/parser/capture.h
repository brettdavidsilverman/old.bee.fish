#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "match.h"
#include "match-ptr.h"
#include "m.h"

using namespace std;

namespace bee::fish::parser {
   
   class Capture : public M
   {
   public:
      BString _value;
      BString& _valueRef;
   public:
      Capture(
         const MatchPtr& match
      ) :
         M(match),
         _valueRef(_value)
      {
         _valueRef.clear();
      }
      
      Capture(
         const MatchPtr& match,
         BString& value
      ) :
         M(match),
         _valueRef(value)
      {
         _valueRef.clear();
      }
      
      Capture(const Capture& source) :
         M(source),
         _valueRef(source._valueRef)
      {
         _valueRef.clear();
      }
      
   
      virtual bool match(Char character)
      {
         if ( character != BString::EndOfFile )
             _valueRef.push_back(character);
         
         return M::match(character, *_item);
      }
      
      virtual BString value() const
      {
         return _valueRef;
      }
   
      virtual MatchPtr copy() const
      {
         return new Capture(*this);
      }
   
      virtual void write(ostream& out) const
      {
         out << "Capture";
         
         writeResult(out);
         
         out << "("
             << *_item
             << ")";
      }
   
   };



}

#endif


