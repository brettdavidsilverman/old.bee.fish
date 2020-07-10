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
   
protected:
   vector<Match*> _inputs;
   
public:
   static const int eof = -1;
   
   template<typename... T>
   Match(T*... inputs) :
      _inputs{ inputs... }
   {
   }
   
   const vector<Match*>& inputs() const {
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
   
   virtual bool match(int character) {
      if (character != Match::eof)
         _value += (char)character;
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
         
         if (matched)
            ++i;
            
         if (success() != nullopt)
            return i;
      }
      
      if (end && success() == nullopt)
         match(Match::eof);
         
      return i;
      
   }
   
   virtual optional<bool> success() const {
      return _success;
   }

   
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
      for (auto
              it = items.cbegin();
              it != items.cend();
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


