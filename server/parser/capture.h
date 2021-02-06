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
   protected:
      Match _match;
      string _value;
   
      typedef
         std::function<void (Capture&)>
         Callback;
      
      Callback _onsuccess;
   public:
  
      Capture(
         const Match& match,
         Callback onsuccess = NULL
      ) :
         _match(match),
         _onsuccess(onsuccess)
      {
      }
   
      Capture(
         const Match& match,
         string& value
      ) :
         _match(match),
         _onsuccess(
            [&value](Capture& item)
            {
               value = item.value();
            }
         )
      {
      }

      Capture(const Capture& source) :
         _match(source._match),
         _onsuccess(source._onsuccess)
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
             _value += (char)character;

         if (_match.result() == true)
            success();
         else if (_match.result() == false)
            fail();
         
         
         return matched;
      }
   
      virtual string& value()
      {
         return _value;
      }
   
      virtual Match* copy() const
      {
         return new Capture(*this);
      }
   
      virtual void success()
      {
         Match::success();
     
         if (_onsuccess)
            _onsuccess(*this);
         
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


