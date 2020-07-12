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
   
   class Inputs : public vector<Match*> {
   public:
      Inputs(initializer_list<Match*> inputs) :
         vector<Match*>(inputs)
      {
      }
      
      Inputs(const Inputs& source)
      {
         for (auto it = source.cbegin();
                   it != source.cend();
                 ++it)
         {
            Match* item = *it;
            push_back(item->copy());
         }
      }
      
   } _inputs;
   
   vector<Match*>::const_iterator
      _inputs_iterator;

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
      _inputs(source._inputs),
      _success(nullopt),
      _inputs_iterator(
         _inputs.cbegin()
      )
   {
      
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
      return true;
   }

   virtual size_t read(
      istream& in,
      bool match_eof = false
   )
   {
      char c;
      while (
         in.get(c) && 
         _success == nullopt
      )
      {
         cout << c;
         match(c);
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
      string::const_iterator index;

      for (index  = str.cbegin();
           index != str.cend();
           ++index)
      {
         char character = *index;
         
         matched =
            match(character);
         
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
   
   Match* operator [] (size_t index) {
      return _inputs[index];
   }
};


}

#endif


