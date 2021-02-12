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
      string& _valueReference;
   public:
      Capture(
         const Match& match
      ) :
         _match(match),
         _valueReference(_value)
      {
      }
      
      Capture(
         const Match& match,
         string& value
      ) :
         _match(match),
         _valueReference(value)
      {
      }
      
      Capture(const Capture& source) :
         _match(source._match),
         _valueReference(source._valueReference)
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
             _valueReference += (char)character;

         if (_match.result() == true)
            success();
         else if (_match.result() == false)
            fail();
         
         
         return matched;
      }
   
      virtual string& value()
      {
         return _valueReference;
      }
      
      virtual Match& item()
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
   
   };



}

#endif


