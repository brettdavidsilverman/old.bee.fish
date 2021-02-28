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
   private:
      string _value;
   public:
      string& _valueRef;
      
   public:
      Capture(
         const MatchPtr& match
      ) :
         Match(match),
         _valueRef(_value)
      {
      }
      
      Capture(
         const MatchPtr& match,
         string& value
      ) :
         Match(match),
         _valueRef(value)
      {
      }
      
      Capture(const Capture& source) :
         Match(source._item),
         _valueRef(source._valueRef)
      {
      }
   
      virtual ~Capture()
      {
      }
   
      virtual bool match(int character)
      {
         if ( character != Match::EndOfFile )
             _valueRef += (char)character;
         
         return Match::match(character);
      }
      
      virtual string& value()
      {
         return _valueRef;
      }
   
      virtual Match* copy() const
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


