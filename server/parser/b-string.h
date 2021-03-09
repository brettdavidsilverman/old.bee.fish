#ifndef BEE_FISH_PARSER__B_STRING_H
#define BEE_FISH_PARSER__B_STRING_H

#include <iostream>
#include <string>
#include <sstream>

#include "utf-8.h"

namespace bee::fish::parser {

   
   class BString :
      public vector<Char>
   
   {
   public:
      inline static const Char EndOfFile =
         UTF8Character::EndOfFile;
      
      // empty string
      BString()
      {
      }
      
      // utf-8 string 
      BString(const std::string& string)
      {
         UTF8Character utf8;
         
         for (const char& c : string)
         {
            utf8.match(c);
            if (utf8._result)
            {
               push_back(utf8._character);
               utf8.reset();
            }
         }

      }
      
      BString(const char* cstring) :
         BString(std::string(cstring))
      {
      }
      
      
      // wide string 
      BString(const std::wstring& wstring)
      {
         for (const wchar_t& wc : wstring)
         {
            push_back((Char)wc);
         }
      }
      
      // utf-8 character
      BString(const Char& character)
      {
         push_back(character);
      }
   
      friend ostream& operator <<
      (ostream& out, const BString& bstr)
      {
         for (auto c : bstr)
         {
            UTF8Character::write(out, c);
         }
         
         return out;
      }
      
      BString& operator += (const string& rhs)
      {
         BString string(rhs);
         insert(end(), string.begin(), string.end());
         return *this;
      }
      
      operator std::string () const
      {
         stringstream stream;
         stream << *this;
         return stream.str();
      }
      
      static void writeEscaped(
         ostream& out,
         const BString& string
      )
      {
         for (const Char& character : string)
            writeEscaped(out, character);
      }

      static void writeEscaped(
         ostream& out,
         const Char& character
      )
      {
         
         switch (character)
         {
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
         case BString::EndOfFile:
            out << "{-1}";
            break;
         default:
            UTF8Character::write(out, character);
         }
      }

   };
   
   inline bool operator ==
   (const BString& lhs, const BString& rhs)
   {
      return (
         (vector<Char>)(lhs) ==
         (vector<Char>)(rhs)
      );
   }
      
   inline bool operator !=
   (const BString& lhs, const BString& rhs)
   {
      return (
         (vector<Char>)(lhs) !=
         (vector<Char>)(rhs)
      );
   }
   
}

#endif


