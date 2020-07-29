#ifndef BEE_FISH_PARSER__LAZY_LOAD_H
#define BEE_FISH_PARSER__LAZY_LOAD_H

#include "match.h"

namespace bee::fish::parser {

template<class T>
class LazyLoad : public Match
{
protected:
   T* _match;

public:
   LazyLoad() : Match() {
      _match = NULL;
   }
   
   virtual bool match(int character) {
      if (_match == NULL)
         _match = new T();
      return _match->match(character);
   }
   
   ~LazyLoad() {
      if (_match)
      {
         delete _match;
         _match = NULL;
      }
   }
   
   virtual optional<bool>& success() 
   {
      if (!_match)
         return _success;
         
      return _match->success();
   }
   
   virtual const optional<bool>& success() const
   {
      if (!_match)
         return _success;
         
      return _match->success();
   }
   
   virtual const string& value() const {
      if (!_match)
         return _value;
      return _match->value();
   }
   
   virtual string& value()
   {
      if (!_match)
         return _value;
         
      return _match->value();
   }
    
   virtual vector<Match*>& inputs()
   {
      if (!_match)
         return _inputs;
         
      return _match->inputs();
   }
   
   virtual const vector<Match*>& inputs() const
   {
      if (!_match)
         return _inputs;
         
      return _match->inputs();
   }
};

}

#endif