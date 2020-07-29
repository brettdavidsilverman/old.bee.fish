#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <array>
#include <map>
#include <sstream>

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
   static const int endOfFile = -1;
   
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
      if (character != Match::endOfFile) {
         _value += (char)character;
      }
      

      return true;
   }

   virtual bool read(
      istream& in,
      bool endOfFile = true
   )
   {
#ifdef DEBUG
      cerr << "{";
#endif

      bool matched;
      
      if (in.eof())
         return endOfFile;
         
      
      int character = in.get();
      
      while (!in.eof())
      {

         matched = match(character);
#ifdef DEBUG
         if (matched)
            writeCharacter(cerr, character);
#endif
         if (success() != nullopt)
            break;
            
         if (matched) {
            // Matched this character,
            // read the next character.
            character = in.get();
         }

      }
         
      if ( success() == nullopt &&
           endOfFile &&
           in.eof()
         )
      {
         
         matched = match(Match::endOfFile);
#ifdef DEBUG
         if (matched)
            writeCharacter(cerr, Match::endOfFile);
#endif
    }
      
#ifdef DEBUG
      cerr << "}";
#endif
      return (success() == true);
   }
   
   virtual bool read(const string& str, bool endOfFile = true)
   {
      
      istringstream in(str);
      
      return read(in, endOfFile);
      
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
      case Match::endOfFile:
         out << "-1";
         break;
      default:
         out << (char)character;
      }
   }
};



}

#endif

