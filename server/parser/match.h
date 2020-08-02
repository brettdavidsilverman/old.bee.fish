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
   optional<bool> _success = nullopt;
 
   string _value = "";
   wstring _wvalue = L"";
 
   vector<Match*> _inputs;
   
   Match()
   {
   }
   
public:
   static const int endOfFile = -1;
   
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
            write(cerr, character);
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
            write(cerr, Match::endOfFile);
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
   
   virtual optional<bool> success() 
   {
      return _success;
   }
 
   virtual void onsuccess()
   {
   }
   
   virtual void setSuccess(optional<bool> value)
   {
      _success = value;
      if (_success)
         onsuccess();
   }
   
   virtual string& value()
   {
      return _value;
   }
   
   virtual wstring& wvalue()
   {
      return _wvalue;
   }
   
   friend ostream& operator <<
   (ostream& out, Match& match) 
   {

      match.write(out);
   
      return out;

   }
   
   virtual void write(ostream& out) 
   {
      
      write(out, success());
 
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
   
   virtual Match& operator[]
   (size_t index)
   {
      return *(inputs()[index]);
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

   void write(ostream& out, optional<bool> ok)
   {
      if (ok == true)
         out << "1";
      else if (ok == false)
         out << "0";
      else
         out << "?";
   }
};



}

#endif


