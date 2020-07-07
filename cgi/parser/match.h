#ifndef MATCH_H
#define MATCH_H

#include <string>
#include <iostream>
#include <optional>
#include <vector>

using namespace std;

namespace Bee::Fish::Parser {

class Match {

private:
   optional<bool> _success = nullopt;
   string _value = "";
   string _buffer = "";
   char _character = '\0';
   vector<Match*> _inputs;
   
public:
   
   template<typename... T>
   Match(T*... inputs) :
      _inputs{ inputs... }
   {
   }
   
   vector<Match*>& inputs() {
      return _inputs;
   }
   
   virtual ~Match() {
  
      for (vector<Match*>::iterator
              it = _inputs.begin();
              it != _inputs.end();
              ++it)
      {
         delete *it;
      }
   }
   
   virtual bool match(char character) = 0;

   size_t read(string str, bool end = true) {
     
      bool matched;
      size_t i;
      for (i = 0;
           i < str.length();
           )
      {
         _character = str[i];
         
         matched =
            match(_character);
         
         if (matched) {
         
            _buffer += _character;
            
            cout << "{"
                 << _character
                 << "}";
         }
         
         if (success() == true)
            _value += _buffer;
            
         if (success()
             != nullopt) {
            _buffer = "";
            return i;
         }
         
         if (matched)
            ++i;
      }
      
      if (end)
         readEnd();
         
      return i;
      
   }
   
   void readEnd() {
   }
   
   optional<bool> success() const {
      return _success;
   }

protected:
   
   virtual void
   setSuccess(optional<bool> value) {
      if (value != _success)
      {
         _success = value;
         if (_success == true) {
            onsuccess();
         }
      }
   }
   
   void onsuccess() {
   }
   
   
public:

   virtual string value() const {
      return _value;
   }
   

};


}

#endif


