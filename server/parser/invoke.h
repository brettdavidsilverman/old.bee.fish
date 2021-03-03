#ifndef BEE_FISH_PARSER__INVOKE_H
#define BEE_FISH_PARSER__INVOKE_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include <functional>
#include "capture.h"

using namespace std;

namespace bee::fish::parser {

   
   
   class Invoke : public Capture
   {

      typedef void (*Callable)(MatchPtr match);
      std::function<void(MatchPtr)> _function;
      
   public:
   
      Invoke(
         MatchPtr match,
         std::function<void(MatchPtr)> func
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
         _function(Match::match());
         
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


