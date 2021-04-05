#ifndef BEE_FISH_PARSER__INVOKE_H
#define BEE_FISH_PARSER__INVOKE_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include <functional>

using namespace std;

namespace bee::fish::parser {

   
   
   class Invoke : public Match
   {
   public:
   
      typedef std::function<void(Match*)> Function;
      Function _function;
      
   public:
   
      Invoke()
      {
      }
      
      Invoke(
         Match* match,
         Function func
      ) :
         _function(func)
      {
         _match = match;
      }
      
      Invoke(const Invoke& source) :
         Match(source),
         _function(source._function)
      {
      }
 
      virtual void success()
      {
         Match::success();
         if (_function)
            _function(_match);
         
      }
      
      virtual Match* copy() const
      {
         return new Invoke(*this);
      }
   
      virtual void write(
         ostream& out,
         size_t tabIndex = 0
      ) const
      {
         out << tabs(tabIndex) << "Invoke";
         
         writeResult(out);
         
         if (_match) 
            out << "("
                << *_match
                << ")";
         else
            out << "(?)";
      }
   
   };



}

#endif

