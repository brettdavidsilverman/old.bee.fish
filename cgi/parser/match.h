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
   
   virtual bool match(char character) {
      _value += character;
      return true;
   }

   size_t read(const string& str, bool end = true) {
     
      bool matched;
      size_t i;
      for (i = 0;
           i < str.length();
           )
      {
         char character = str[i];
         
         matched =
            match(character);
         
         if (matched) {
            cout << "{"
                 << character
                 << "}";
         }
            
         if (success() != nullopt)
            return i;
         
         if (matched)
            ++i;
      }
      
      if (end)
         readEnd();
         
      return i;
      
   }
   
   virtual void readEnd() {
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
   
   virtual void onsuccess() {
   }
   
   
public:

   virtual const string value() const {
      return _value;
   }
   
   virtual const string word(
      const vector<Match*>& items
   ) const
   {
      string word = "";
      for (vector<Match*>::const_iterator
              it = items.begin();
              it != items.end();
              ++it)
      {
         const Match* item = *it;
         if (item->success() == true)
            word += item->value();
      }
      
      return word;
   }
   

};


}

#endif


