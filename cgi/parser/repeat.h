#include "match.h"

namespace Bee::Fish::Parser {

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
   
   virtual ~Repeat() {
   
      for (vector<Match*>::iterator
              it = _items.begin();
              it != _items.end();
              ++it)
      {
         delete *it;
      }
      
      delete _match;
   }
   
   virtual bool match(int character) {
   
      bool matched =
         _match->match(character);
         
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
            setSuccess(true);
         }
         else {
            setSuccess(false);
         }
      }
      
      return matched;
      
   }
  
   virtual const vector<Match*>& items() const {
       return _items;
   }
   
   virtual const string value() const {
      return Match::word(items());
   }
   
   virtual void addItem(T* match) {
      
   }
   
};


};
