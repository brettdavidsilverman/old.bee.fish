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
      BString _value;

   public:
      MatchPtr _item;
      BString& _valueRef;
      
   public:
      Capture(
         const MatchPtr& match
      ) :
         _item(match),
         _valueRef(_value)
      {
      }
      
      Capture(
         const MatchPtr& match,
         BString& value
      ) :
         _item(match),
         _valueRef(value)
      {
      }
      
      Capture(const Capture& source) :
         _item(source._item->copy()),
         _valueRef(source._valueRef)
      {
      }
   
      virtual bool match(Char character)
      {
         if ( character != Match::EndOfFile )
             _valueRef.push_back(character);
         
         return Match::match(character, *_item);
      }
      
      virtual BString& value()
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


