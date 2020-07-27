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
private:
   optional<bool> _success;
 
protected:
   string _value;
 
   vector<Match*> _inputs;
   
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
         
         matched = match(c);
         
         if (matched)
            in.get(c);
      }
         
      if ( success() == nullopt &&
           match_eof &&
           in.eof()
         )
      {
         
         matched = match(Match::eof);
         
      }
      
      return (success() == true);
   }
   
   virtual bool read(const string& str, bool end = true) {
      
      bool matched;
      
      for (auto
              index  = str.cbegin();
              index != str.cend();
          )
      {
         char character = *index;
         
         matched =
            match(character);
         
         if (success() != nullopt)
            break;
                   
         if (matched)
            ++index;
      }
      
      if (end && (success() == nullopt))
      {

         matched = match(Match::eof);
         
      }
      
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

template<class T>
class _Match : public Match
{
protected:
   T* _match;
   
public:
   _Match() : Match() {
      _match = NULL;
   }
   
   virtual bool match(int character) {
      if (_match == NULL)
         _match = new T();
      return _match->match(character);
   }
   
   ~_Match() {
      if (_match)
      {
         delete _match;
         _match = NULL;
      }
   }
    
   
};

}

#endif


