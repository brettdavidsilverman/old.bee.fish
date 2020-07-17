#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H


#include <iostream>
#include <string>
#include <optional>
#include <array>

using namespace std;

ostream& operator <<
(ostream& out,
const optional<bool>& success);

namespace bee::fish::parser {

class Match {

protected:
   optional<bool> _success = nullopt;
   string _value = "";
   
   virtual void
   set_success(optional<bool> value)
   {
      _success = value;
   }
   
public:
   static const int eof = -1;
   

   Match()
   {
   }
   
   Match(const Match& source) {
      _success = nullopt;
      _value = "";
   }
   
   virtual ~Match() {
   }
   
   virtual bool
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
      bool matched;
      
      if (!in.eof())
         in.get(c);
         
      while ( _success == nullopt &&
             !in.eof()
            )
      {
         cerr << "{" << c << ":";
         
         matched = match(c);
         
         cerr << _success << "," <<
                  matched << "}";
         
         if (matched)
            in.get(c);
      }
         
      if ( _success == nullopt &&
           match_eof &&
           in.eof()
         )
      {
         cerr << "{-1:";
         matched = match(Match::eof);
         cerr << _success << matched
              << "}";
      }
      
      return (_success == true);
   }
   
   virtual bool read(const string& str, bool end = true) {
      
      
      bool matched;
      
      for (auto
              index  = str.cbegin();
              index != str.cend();
          )
      {
         char character = *index;
         
         cerr << "{" << character << ":";
         
         matched =
            match(character);

         cerr << _success;
         cerr << ",";
         cerr << matched;
         cerr << "}";
         
         if (_success != nullopt)
            break;
                   
         if (matched)
            ++index;
      }
      
      if (end && (_success == nullopt))
      {
         cerr << "{-1:";
         
         matched = match(Match::eof);
         
         cerr << _success;
         cerr << ",";
         cerr << matched;
         cerr << "}";
      }
      
      return (_success == true);
      
   }
   
   virtual optional<bool> success() const {
      return _success;
   }
   
   friend ostream& operator <<
   (ostream& out, const Match& match) 
   {

      match.write(out);
   
      return out;

   }
   
   virtual void write(ostream& out) const 
   {
   
      out << success();

      if (success() == true)
      {
         out << "["
             << value()
             << "]";
      }
   }
   
   virtual Match* copy() const = 0;
   
public:

   
   virtual string value() const {
      return _value;
   }
   
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
   
   virtual Match&
   operator[] (size_t index) {
      throw std::out_of_range
      ("Match::[]");
   }


};


}

#endif


