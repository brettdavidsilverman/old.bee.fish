#ifndef BEE_FISH_PARSER__CAPTURE_H
#define BEE_FISH_PARSER__CAPTURE_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "match.h"

using namespace std;

namespace bee::fish::parser {
   
   class Capture : public Match
   {
   public:
      BString& _valueRef;
      BString _value;
      
   public:
      Capture() :
         _valueRef(_value)
      {
      }
      
      Capture(
         Match* match
      ) :
         _valueRef(_value)
      {
         _match = match;
      }
      
      Capture(
         Match* match,
         BString& value
      ) :
         _valueRef(value)
      {
         _match = match;
      }
      
      Capture(const Capture& source) :
         Match(source),
         _valueRef(_value)
      {
      }
      
      virtual void capture(const Char& character)
      {
         _valueRef.push_back(character);
      }
      
      virtual const BString& value() const
      {
         return _valueRef;
      }
   
      virtual Match* copy() const
      {
         return new Capture(*this);
      }
   
      virtual void write(
         wostream& out,
         size_t tabIndex = 0
      ) const
      {
         BString tabs = Match::tabs(tabIndex);
         
         out << tabs << "Capture";
         
         writeResult(out);
         
         out << endl
             << tabs
             << "("
             << endl;
         _match->write(out, tabIndex + 1);
         out << endl
             << tabs
             << ")";
      }
   
   };



}

#endif


