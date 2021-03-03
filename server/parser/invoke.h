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

   typedef void (*Callable)(Capture&);
   
   template<typename F = Callable>
   class Invoke : public Capture
   {

      
      F _function;
      
   public:
      Invoke(
         const Match& match,
         F func
      ) :
         Capture(match),
         _function(func)
      {
      }
      
      Invoke(const Invoke& source) :
         Capture(source),
         _function(source._function)
      {
      }
   
      virtual ~Invoke()
      {
      }
   
      virtual void success()
      {
         Match::success();
         Invoke& item = *this;
         _function(item);
         
      }
      
      virtual MatchPtrBase copy() const
      {
         return make_shared<Invoke>(*this);
      }
   
      virtual void write(ostream& out) const
      {
         out << "Invoke";
         
         writeResult(out);
         
         out << "("
             << *_match
             << ")";
      }
   
   };



}

#endif


