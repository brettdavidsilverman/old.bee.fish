#ifndef BEE_FISH_PARSER__INVOKE_H
#define BEE_FISH_PARSER__INVOKE_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "capture.h"

using namespace std;

namespace bee::fish::parser {
   
   class Invoke : public Capture
   {

   public:
      Invoke(
         const Match& match
      ) :
         Capture(match)
      {
      }
      
      Invoke(const Invoke& source) :
         Capture(source)
      {
      }
   
      virtual ~Invoke()
      {
      }
   
      virtual void success()
      {
         F(*this);
      }
      
      virtual Match* copy() const
      {
         return new Invoke(*this);
      }
   
      virtual void write(ostream& out) const
      {
         out << "Invoke";
         
         writeResult(out);
         
         out << "("
             << _match
             << ")";
      }
   
   };



}

#endif


