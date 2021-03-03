#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "match.h"
#include "match-ptr.h"

using namespace std;

namespace bee::fish::parser {
   
   class Capture : public Match
   {
   public:
      BString& _valueRef;
      
   public:
      Capture(
         MatchPtr match
      ) :
         Match(match),
         _valueRef(_value)
      {
      }
      
      Capture(
         MatchPtr match,
         BString& value
      ) :
         Match(match),
         _valueRef(value)
      {
      }
      
      Capture(const Capture& source) :
         Match(source),
         _valueRef(source._valueRef)
      {
      }
      
   
      virtual bool match(Char character)
      {
         if ( character != BString::EndOfFile )
             _valueRef.push_back(character);
         
         return Match::match(character, *_match);
      }
      
      virtual BString value() const
      {
         return _valueRef;
      }
   
      virtual MatchPtrBase copy() const
      {
         return make_shared<Capture>(*this);
      }
   
      virtual void write(ostream& out) const
      {
         out << "Capture";
         
         writeResult(out);
         
         out << "("
             << *_match
             << ")";
      }
   
   };



}

#endif


