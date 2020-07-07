#include "match.h"

namespace Bee::Fish::Parser {

template<class T>
class Repeat : public Match {
private:
   T* _match;
   vector<Match*> _values;
   string _value = "";
  
public:

   Repeat() : Match()
   {
      _match = new T();
   }
   
   virtual ~Repeat() {
   
      for (vector<Match*>::iterator
              it = _values.begin();
              it != _values.end();
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

         _values.push_back(
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
            _values.push_back(_match);
         }
      }
      checkSuccess();
   }
   
   void checkSuccess() {
      
      if (_values.size() > 0) {
         setSuccess(true);
      }
      else {
         setSuccess(false);
      }
   }
   
   virtual vector<Match*>& items() {
       return _values;
   }
   
};


};
