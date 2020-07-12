#ifndef BEE_FISH_PARSER__REPEAT_H
#define BEE_FISH_PARSER__REPEAT_H

#include "match.h"

namespace bee::fish::parser {

template<class T>
class Repeat : public Match {
private:
   T* _match;
   string _value = "";
   
protected:
   vector<Match*> _items;
   
  
public:

   Repeat() : Match()
   {
      _match = new T();
   }
   
   Repeat(const Repeat& source) :
      Match(source)
   {
      _match = new T();
   }
   
   virtual ~Repeat() {
   
      for (vector<Match*>::iterator
              it = _items.begin();
              it != _items.end();
              ++it)
      {
         Match* item = *it;
         delete item;
      }
      
      delete _match;
   }
   
   virtual optional<bool>
   match(int character)
   {

      optional<bool> matched =
         _match->match(character);
         
      if (matched != false)
         Match::match(character);
         
      if (_match->success() == true) {
      
         _items.push_back(
            _match
         );
         
         addItem(_match);
            
         _match = new T();
           
      }
      else if (
         _match->success() == false ||
         character == Match::eof
      )
      {
         if (_items.size() > 0) {
            _success = true;
         }
         else {
            _success = false;
         }
      }
      
      return _success;
      
   }
  
   virtual const vector<Match*>& items() const {
       return _items;
   }
   
   virtual void addItem(T* match) {
      
   }
   
   virtual void write(ostream& out) const {
      out << "Repeat";
   }
   
   virtual Match* copy() {
      Repeat* copy = new Repeat(*this);
      return copy;
   }
   
};


};

#endif
