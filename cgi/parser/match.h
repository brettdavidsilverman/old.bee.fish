#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <array>
#include <map>

using namespace std;

ostream& operator <<
(ostream& out,
const optional<bool>& success);

namespace bee::fish::parser {

class Capture;

class Match {
private:
   optional<bool> _success;
 
protected:
   string _value;
   string _name;
   vector<Match*> _inputs;
   map<string, Match*> _capture;
   friend class Capture;
   
   virtual void
   set_success(optional<bool> value)
   {
      _success = value;
   }
   
   
protected:

   Match()
   {
      _success = nullopt;
      _value = "";
   }
   
public:
   static const int eof = -1;
   
   template<typename ...T>
   Match(T*... inputs) :
      _inputs{inputs...}
   {
      _success = nullopt;
      _value = "";
   }
   
   virtual ~Match() {
      for (auto
             it = _inputs.cbegin();
             it != _inputs.cend();
           ++it)
      {
         delete (*it);
      }
   }
   
   virtual bool match(int character)
   {
      if (character != Match::eof) {
         _value += (char)character;
      }
      return true;
   }

   virtual bool read(
      istream& in,
      bool match_eof = false
   )
   {
   
      cerr << endl << "{" << endl;
      
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
      
      cerr << endl << "}" << endl;
      
      return (success() == true);
   }
   
   virtual bool read(const string& str, bool end = true) {
      
      cerr << endl << "{" << endl;
      
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
      
      cerr << endl << "}" << endl;
      
      return (success() == true);
      
   }
   
   virtual optional<bool> success() const {
      return _success;
   }
   
   virtual const string& value() const {
      return _value;
   }
   
   virtual const string& name() const
   {
      return _name;
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
      for (auto it = inputs().cbegin();
                it != inputs().cend();
          )
      {
         out << **it;
         
         if (++it != inputs().cend())
            out << ",";
      }
   }
protected:
   virtual vector<Match*>& inputs()
   {
      return _inputs;
   }
   
   virtual const vector<Match*>& inputs() const
   {
      return _inputs;
   }
   
public:
   virtual Match&
   operator[] (size_t index) const {
      return *(_inputs[index]);
   }
   
 
protected:
   void write_character(ostream& out, int character) const
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


