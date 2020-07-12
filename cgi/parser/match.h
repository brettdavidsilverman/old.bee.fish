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
   optional<bool> _success = nullopt;
   vector<Match*> _inputs;
   vector<Match*>::const_iterator _inputs_iterator;

public:
   static const int eof = -1;
   
   template<class... Types>
   Match(Types*... args) :
      _inputs{args...}
   {
      cout << "Match::Match("
           << _inputs.size() 
           << ")"
           << endl;
           
      _inputs_iterator = _inputs.cbegin();
         
   }
   
   Match(const Match& source) {
      _inputs = source._inputs;
      _inputs_iterator = _inputs.cbegin();
   }
   
   virtual ~Match() {
     // cout << "Match::~Match()" << endl;
      for (auto it  = _inputs.cbegin();
                it != _inputs.cend();
              ++it)
      {
         Match* match = *it;
         delete match;
      }
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
      for (index  = str.cbegin();
           index != str.cend();
           ++index, ++i)
      {
         char character = *index;
         
         matched =
            match(character);

         if (matched == false || 
             _success != nullopt)
            break;
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
   
   virtual void write(ostream& out) const {
      out << "Match";
   }
  
   virtual Match* copy()  = 0;
   
   
public:

   virtual vector<Match*> inputs() const {
      return _inputs;
   }
   
   virtual const string& value() const {
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
   
   Match* operator [] (size_t index) {
      return _inputs[index];
   }
};


}

#endif


