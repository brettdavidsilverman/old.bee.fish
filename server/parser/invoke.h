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

      typedef std::function<void(MatchPtr)> Function;
      Function _function;
      bool _setup;
      
   public:
   
      Invoke() :
         _setup(false)
      {
      }
      
      Invoke(
         MatchPtr match,
         Function func
      ) :
         Match(match),
         _function(func),
         _setup(true)
      {
      }
      
      Invoke(const Invoke& source) :
         Match(source),
         _function(source._function),
         _setup(false)
      {
      }
   
      
      virtual void setup()
      {
         _setup = true;
      }
      
      virtual bool match(Char character) 
      {
         if (!_setup)
            setup();
            
         return Match::match(character);
      }
   
      void setMatch(MatchPtr match, Function func = nullptr)
      {
         Match::setMatch(match);
         _function = func;
      }
      
      virtual void success()
      {
         Match::success();
         if (_function)
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


