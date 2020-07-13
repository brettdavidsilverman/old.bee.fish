#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H


#include <iostream>
#include <string>
#include <optional>
#include <array>
#include <initializer_list>

using namespace std;

namespace bee::fish::parser {

class Match {

private:
   string _value = "";

protected:
   optional<bool> _success = nullopt;
   
   vector<Match*> _inputs;
   
   vector<Match*>::const_iterator
      _inputs_iterator;

   virtual void
   set_success(optional<bool> value)
   {
      _success = value;
   }
   
public:
   static const int eof = -1;
   
   Match(initializer_list<Match*> input) :
      _inputs(input)
   {
      _inputs_iterator =
         _inputs.cbegin();
   }
   
   Match() : Match({})
   {
   }
   
   Match(const Match& source) :
      _success(nullopt)
   {
      for (auto
             it = source._inputs.cbegin();
             it != source._inputs.cend();
           ++it)
      {
         Match* item = *it;
         _inputs.push_back(
            item->copy()
         );
      }
      
     _inputs_iterator = _inputs.cbegin();
      
   }
   
   virtual ~Match() {
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
      _value += (char)character;
      return true;
   }

   virtual size_t read(
      istream& in,
      bool match_eof = false
   )
   {
      char c;
      optional<bool> matched;
      in.get(c);
      while (
         _success == nullopt &&
         !in.eof()
      )
      {
         cout << "{" << c << "}";
         matched = match(c);
         
         if (matched != false)
            in.get(c);
      }
         
      if (
         _success == nullopt &&
         match_eof &&
         in.eof()
      )
      {
         match(Match::eof);
      }
      
      return (_success == true);
   }
   
   virtual bool read(const string& str, bool end = true) {
     
      optional<bool> matched;
      
      for (auto
              index  = str.cbegin();
              index != str.cend();
          )
      {
         char character = *index;
         
         cout << "{" << character << "}";
         
         matched =
            match(character);
          
         if (matched != false)
            ++index;

         if (_success != nullopt)
            break;
      }
      
      if (end && (_success == nullopt))
      {
         match(Match::eof);
      }
      
      return (_success == true);
      
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
   
   Match& operator [] (size_t index) const {
      return *(_inputs[index]);
   }
};


}

#endif


