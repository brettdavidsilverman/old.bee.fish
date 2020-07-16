#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"
namespace bee::fish::parser {

template <class T>
class Repeat : public Match {
private:
   T* _match;
   string _value = "";
   
protected:
   vector<T*> _items;
   
  
public:

   Repeat() : Match()
   {
      _match = new T();
   }
   
   Repeat(const Repeat<T>& source) :
      Match(source)
   {
      _match = (T*)source._match->copy();
   }
   
   virtual ~Repeat() {
   
      for (auto
              it = _items.cbegin();
              it != _items.cend();
              ++it)
      {
         delete (*it);
      }
      
      delete _match;
   }
   
   virtual bool
   match(int character)
   {

      bool matched =
         _match->match(character);
         
      if (matched)
         Match::match(character);
         
      if (_match->success() == true) {
      
         _items.push_back(
            _match
         );
         
         addItem(_match);
            
         _match = new T();
           
      }
      else if
         (
            !matched ||
            character == Match::eof
         )
      {
         set_success(
            (_items.size() > 0)
         );
         
      }
      
      return matched;
      
   }
  
   virtual const vector<T*>& items() const {
       return _items;
   }
   
   virtual void addItem(T* match) {
      
   }
   
   
   virtual Match* copy() const {
      Repeat* copy = new Repeat<T>(*this);
      return copy;
   }
   
};


};

#endif
