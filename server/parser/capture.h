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
   private:
      string _value;
   protected:
      Match _match;
      string& _valueRef;
   public:
   
      Capture(
         const Match& match,
         string& value
      ) :
         _match(match),
         _valueRef(value)
      {
      }
      
      Capture(
         const Match& match
      ) :
         _match(match),
         _valueRef(_value)
      {
      }
      Capture(const Capture& source) :
         _match(source._match),
         _valueRef(source._valueRef)
      {
      }
    
      virtual ~Capture()
      {
      }
   
      virtual bool match(int character)
      {
         bool matched =
            _match.match(character);
      
         if ( matched &&
              character != Match::EndOfFile )
             _valueRef += (char)character;

         if (_match.result() == true)
            success();
         else if (_match.result() == false)
            fail();
         
         
         return matched;
      }
   
      virtual string& value()
      {
         return _valueRef;
      }
      
      virtual Match& item()
      {
         return _match;
      }
      
      virtual const Match& item() const
      {
         return _match;
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
             << _match
             << ")";
      }
      
      virtual void writeResult(ostream& out) const
      {
         Match::writeResult(out);
      }
   
   };



}

#endif


