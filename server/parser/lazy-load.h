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
      return match().match(character);
   }
   
   virtual T* createItem()
   {
      return new T();
   }
   
   ~LazyLoad() {
      if (_match)
      {
         delete _match;
         _match = NULL;
      }
   }
   
   virtual optional<bool> success() 
   {
      return match().success();
   }
 
   
   virtual string& value() {
      return match().value();
   }
   
   virtual wstring& wvalue()
   {
      return match().wvalue();
   }
   
   virtual T& match() {
      if (!_match)
         _match = createItem();
      return *_match;
   }
   
   virtual vector<Match*>& inputs()
   {
      return match().inputs();
   }
   
   virtual void write(ostream& out)
   {
      out << "LazyLoad(";
      Match::write(out);
      out << match();
      out << ")";
   }
   
};

}

#endif