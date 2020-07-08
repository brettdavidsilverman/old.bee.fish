#include "match.h"

namespace Bee::Fish::Parser {

template<class T>
class Repeat : public Match {
private:
   T* _match;
   vector<Match*> _items;
   string _value = "";
  
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
   
   virtual bool match(char character) {
   
      bool matched =
         _match->match(character);
         
      if (_match->success() == true) {

         _items.push_back(
            _match
         );
            
         _match = new T();
           
      }
      else if (_match->success() == false)
      {
         checkSuccess();
         _match = new T();
      }
      
      return matched;
      
   }
   
   virtual void readEnd() {
      Match::readEnd();
      if (_match->success() == nullopt)
      {
         _match->readEnd();
         if (_match->success() == true) {
            _items.push_back(_match);
         }
      }
      checkSuccess();
   }
   
   void checkSuccess() {
      
      if (_items.size() > 0) {
         setSuccess(true);
      }
      else {
         setSuccess(false);
      }
   }
   
   virtual const vector<Match*>& items() const {
       return _items;
   }
   
   virtual const string value() const {
      return Match::word(items());
   }
   
};


};
