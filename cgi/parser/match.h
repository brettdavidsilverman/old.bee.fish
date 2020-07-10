#ifndef MATCH_H
#define MATCH_H

#include <string>
#include <iostream>
#include <optional>
#include <array>

using namespace std;

namespace Bee::Fish::Parser {

class Match {

private:
   string _value = "";
   
protected:
   vector<Match> _inputs;
   optional<bool> _success = nullopt;
   
public:
   static const int eof = -1;
   
   template<typename... T>
   Match(const T&... inputs) :
      _inputs{ inputs... }
   {
      cout << "Match::Match("
           << _inputs.size() 
           << ")"
           << endl;
   }
   
   virtual optional<bool>
   match(int character)
   {
   
      if (character != Match::eof) {
         _value += (char)character;
         cout << "{"
              << (char)character
              << "}";
      }
      
      return true;
   }

   size_t read(const string& str, bool end = true) {
     
      optional<bool> matched;
      string::const_iterator index;
      size_t i = 0;
      for (index = str.begin();
           (index != str.end() &&
           _success == nullopt);
           ++index, ++i
           )
      {
         char character = *index;
         
         matched =
            match(character);

      }
      
      if (end && (_success == nullopt))
      {
         match(Match::eof);
      }
      
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
      cout << "!";
   }
   
   
public:

   virtual const vector<Match>& inputs() const {
      return _inputs;
   }
   
   Match& operator[](size_t index) {
      return _inputs[index];
   }
   
   virtual const string& value() const {
      return _value;
   }
   /*
   virtual const string word(
      const vector<Match>& items
   ) const
   {
      string word = "";
      for (auto
              it = items.cbegin();
              it != items.cend();
              ++it)
      {
         const Match& item = *it;
         if (item.success() == true)
            word += item.value();
      }
      
      return word;
   }
   
*/
};


}

#endif


