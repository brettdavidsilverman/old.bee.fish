#ifndef BEE_FISH_PARSER__MATCH_H
#define BEE_FISH_PARSER__MATCH_H

#include <iostream>
#include <string>
#include <optional>
#include <map>
#include <sstream>
#include "../id/id.h"

using namespace std;

inline ostream& operator <<
(ostream& out, optional<bool> ok)
{
   if (ok == true)
      out << "1";
   else if (ok == false)
      out << "0";
   else
      out << "?";
         
   return out;
}

using namespace bee::fish::server;

namespace bee::fish::parser {

class Match {
protected:

   string _value = "";
   
   optional<bool> _success = nullopt;

public:
   static const int endOfFile = -1;
   
   vector<Match*> _inputs;
   
   template<typename ...T>
   Match(T*... inputs) :
      _inputs{inputs...}
      
   {

   }
   
   virtual ~Match() {
      
      for (auto
             it = _inputs.cbegin();
             it != _inputs.cend();
           ++it)
      {
         Match* child = *it;
         if (child)
         {
            delete child;
         }
      }
      
   }
   
   virtual bool match
   (int character, optional<bool>& success)
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
      int character;
      _success = nullopt;
      
      while (!in.eof())
      {
         character = in.get();
        
         match(character, _success);
#ifdef DEBUG
         cout << (char)character;
#endif
         if (_success != nullopt)
            break;
      }

      if ( _success == nullopt &&
           endOfFile &&
           in.eof()
         )
      {
         match(Match::endOfFile, _success);
      }
     
      return (_success == true);
   }
   
   virtual bool read(const string& str, bool endOfFile = true)
   {
      
      istringstream in(str);
      
      return read(in, endOfFile);
      
   }
   
   virtual optional<bool> success()
   {
      return _success;
   }
   
   virtual void onsuccess()
   {
      _success = true;
   }
   
   virtual void onfail()
   {
      _success = false;
   }
  
   
   virtual string& value()
   {
      return _value;
   }
 
   friend ostream& operator <<
   (ostream& out, Match& match) 
   {

      match.write(out);
   
      return out;

   }
   
   virtual void write(ostream& out) 
   {
      
      out << "{" 
          << name() 
          << ":{ok:\""
          << _success
          << "\"";
 
      for (auto it = _inputs.cbegin();
                it != _inputs.cend();
                ++it
          )
      {
         out << ',';
         Match* input = *it;
         if (input)
            out << *input;
         else
            out << "null";
      }
      
      out << '}'
          << '}';
   }
   
   virtual string name() {
      return "Match";
   }
   
   virtual Match& operator[]
   (size_t index)
   {
      return *(_inputs[index]);
   }
   
 
public:

   virtual void write(ostream& out, const wstring& wstr)
   {
      for (const wchar_t wc : wstr) {
         char cHigh = (wc & 0xFF00) >> 8;
         char cLow = (wc & 0x00FF);
         if (cHigh)
         {
            out << "\\u";
            out << std::hex;
            out << cHigh;
            out << cLow;
            out << std::dec;
         }
         else
         {
            write(out, cLow);
         }
      }
   }
   
   virtual void write(ostream& out, int character)
   {
      switch (character) {
      case '\"':
         out << "\\\"";
         break;
      case '\\':
         out << "\\\\";
         break;
      case '\b':
         out << "\\b";
         break;
      case '\f':
         out << "\\f";
         break;
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


