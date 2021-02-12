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
   
   class Invoke;
   
   class Invoke : public Capture
   {

      typedef std::function<void(Invoke&)>
         Callable;
         
      Callable _function;
   public:
      Invoke(
         const Match& match,
         Callable func
      ) :
         Capture(match)
      {
         _function = func;
      }
      
      Invoke(const Invoke& source) :
         Capture(source)
      {
         _function = source._function;
      }
   
      virtual ~Invoke()
      {
      }
   
      virtual void success()
      {
         _function(*this);
         Capture::success();
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


