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

class Match {
protected:
   optional<bool> _success;
 
   string _value;
 
   vector<Match*> _inputs;
   
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
      bool matchEOF = false
   )
   {
#ifdef DEBUG
      cerr << "{";
#endif
      char c;
      bool matched;
      
      if (!in.eof())
         in.get(c);
         
      while ( success() == nullopt &&
             !in.eof()
            )
      {
         
         matched = matchNextCharacter(c);

         if (matched) {
            in.get(c);
         }

      }
         
      if ( success() == nullopt &&
           matchEOF &&
           in.eof()
         )
      {
         
         matched = matchNextCharacter(Match::eof);
      }
#ifdef DEBUG
      cerr << "}";
#endif
      return (success() == true);
   }
   
   virtual bool read(const string& str, bool end = true) {
      
      bool matched;
#ifdef DEBUG
      cerr << "{";
#endif
      for (auto
              index  = str.cbegin();
              index != str.cend();
          )
      {
         char character = *index;
         
         matched =
            matchNextCharacter(character);
         
         if (success() != nullopt)
            break;
                   
         if (matched)
            ++index;
      }
      
      if (end && (success() == nullopt))
      {

         matched = matchNextCharacter(
            Match::eof
         );
         
      }
      
#ifdef DEBUG
      cerr << "}";
#endif

      return (success() == true);
      
   }
   
   virtual optional<bool>& success() 
   {
      return _success;
   }
   
   virtual const optional<bool>& success() const
   {
      return _success;
   }
   
   virtual const string& value() const {
      return _value;
   }
   
   virtual string& value()
   {
      return _value;
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
      return *(inputs()[index]);
   }
   
 
protected:
   bool matchNextCharacter(int character)
   {
      bool matched = match(character);
#ifdef DEBUG
      if (matched)
         writeCharacter(cerr, character);
#endif
      return matched;
   }
   
   void writeCharacter(ostream& out, int character) const
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


