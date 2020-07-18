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
private:
   optional<bool> _success;
   Match* _match;
protected:
   
   string _value;
   
   
   virtual void
   set_success(optional<bool> value)
   {
      _success = value;
   }
   
public:
   static const int eof = -1;
   

   Match()
   {
      _success = nullopt;
      _value = "";
      _match = NULL;
   }
   
   Match(const string& name, const Match& source) : Match() 
   {
      _match = source.copy();
   }
   
   virtual Match* copy() const {
      Match* copy = new Match();
      if (_match)
         copy->_match = _match->copy();
      return copy;
   }
   
   virtual ~Match() {
      if (_match)
         delete _match;
   }
   
   virtual bool
   match(int character)
   {
      bool matched = true;
     
      if (_match)
         matched = _match->match(character);
      if (matched)
         _value += (char)character;
      return matched;
   }

   virtual size_t read(
      istream& in,
      bool match_eof = false
   )
   {
   
      cerr << "{";
      
      char c;
      bool matched;
      
      if (!in.eof())
         in.get(c);
         
      while ( success() == nullopt &&
             !in.eof()
            )
      {
         
         write_character(
            cerr,
            c
         );
         
         matched = match(c);
         
         if (matched)
            in.get(c);
      }
         
      if ( success() == nullopt &&
           match_eof &&
           in.eof()
         )
      {
         
         write_character(
            cerr,
            Match::eof
         );
         matched = match(Match::eof);
         
      }
      
      cerr << "}";
      
      return (success() == true);
   }
   
   virtual bool read(const string& str, bool end = true) {
      
      cerr << "{";
      
      bool matched;
      
      for (auto
              index  = str.cbegin();
              index != str.cend();
          )
      {
         char character = *index;
         
         
         write_character(
            cerr,
            character
         );
         
         matched =
            match(character);
         
         if (success() != nullopt)
            break;
                   
         if (matched)
            ++index;
      }
      
      if (end && (success() == nullopt))
      {

         write_character(
            cerr,
            Match::eof
         );
         
         matched = match(Match::eof);
         
      }
      
      cerr << "}";
      
      return (success() == true);
      
   }
   
   virtual optional<bool> success() const {
      if (_match)
         return _match->success();
      else
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
   
private:
   void write_character(ostream& out, int character)
   {
      switch (character) {
      case '\r':
         out << "\\r";
         break;
      case '\n':
         out << "\\n";
         break;
      case '\t':
         out << "\\t";
         break;
      case Match::eof:
         out << "-1";
         break;
      default:
         out << (char)character;
      }
   }
};


}

#endif


