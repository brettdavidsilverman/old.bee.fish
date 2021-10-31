#ifndef BEE_FISH_B_STRING__B_STRING_H
#define BEE_FISH_B_STRING__B_STRING_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <cstring>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <codecvt>
#include <locale>

#ifndef ARDUINO
#include <filesystem>
#endif

#include "data.h"
#include "bit-stream.h"

#ifndef ARDUINO
using namespace std::filesystem;
#endif

namespace bee::fish::b_string {

   inline std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conversion;

   std::string wstr2str(const wstring& wstr)
   {
      return conversion.to_bytes(wstr);
   }

   std::wstring str2wstr(const string& str)
   {
      return conversion.from_bytes(str);
   }
   typedef std::wstring BStringBase;
   //typedef vector<Character> BStringBase;
   
   typedef wchar_t Character;
   
   class BString;
   
   // A string of variable length characters.
   // Can be created from wide string format,
   // and utf-8 format.
   // Once created, holds each character as a
   // power encoded vector of bits
   class BString :
      public BStringBase
   
   {
         
   public:
      
      
      // empty string
      BString()
      {
      }
      
      // standard copy constructor
      BString(const BString& source) :
         BStringBase(source)
      {
      }
      
      // from wstring
      BString(const BStringBase& source) :
         BStringBase(source)
      {
      }
      
      // from data
      BString(const Data& source) :
         BString(fromData(source))
      {
      }

#ifndef ARDUINO
      // from path
      BString(const path& path) :
         BString(string(path))
      {
      }
#endif
      
      // from copy iterators
      BString(
         BStringBase::const_iterator first,
         BStringBase::const_iterator last
      ) :
         BStringBase(first, last)
      {
      }
      
      // from utf-8 string 
      BString(const std::string& str) :
         BString(str2wstr(str))
      {

      }
      
      // c string
      BString(const char* cstring) :
         BString(std::string(cstring))
      {
      }
      
      // c string with length
      BString(const char* cstring, size_t len) :
         BString(std::string(cstring, len))
      {
      }
      
      // Wide c string
      BString(const wchar_t* wstr) :
         BString(wstring(wstr))
      {
      }
      

      Data toData() const
      {
         BitStream stream;
         
         stream << *this;
         
         return stream.toData();
      }
      
      // Stream indexable bits from data
      static BString fromData(const Data& source)
      {

         BitStream stream = BitStream::fromData(source);
         
         BString bString;
         
         stream >> bString;
         
         return bString;
      }
      
      std::string toUTF8() const
      {
         return wstr2str(*this);
      }
      
      virtual ~BString()
      {
      }
      
      BString& operator += (const BString& rhs)
      {
         for (auto character : rhs)
         {
            push_back(character);
         }
         return *this;
      }
      
      BString operator + (const BString& rhs) const
      {
         BString str(*this);
         for (auto character : rhs)
            str.push_back(character);
         return str;
      }
      
      virtual bool operator == (const char* rhs) const
      {

         BString comparison = rhs;

         return (*this == comparison);
      }
      
      virtual bool operator != (const char* rhs) const
      {
         BString comparison(rhs);
         
         return (*this != comparison);
      }
      
      operator std::string () const
      {
         return toUTF8();
      }
      
      
      BString toLower() const
      {
         BString copy;
         for (const Character& c : *this)
            copy.push_back(
               tolower(c)
            );
            
         return copy;
      }

      vector<BString> split(
         const Character& character
      ) const
      {
         BString segment;
         vector<BString> segments;
         
         for (auto c : *this)
         {

            if (c == character)
            {
               segments.push_back(segment);
               segment.clear();
            }
            else
               segment.push_back(c);
         }
         
         segments.push_back(segment);
 
         return segments;
      }
      
      friend wostream& operator <<
      (wostream& out, const BString& str)
      {
         str.write(out);
         return out;
      }
      
      virtual void write(wostream& out) const
      {
         for ( const Character& character : *this )
         {
            out << character;
         }
      }
      
      virtual void writeEscaped(
         wostream& out
      ) const
      {
         for (const Character& character : *this)
            writeEscaped(out, character);
      }

      friend PowerEncoding& operator <<
      ( 
         PowerEncoding& stream,
         const BString& bString
      )
      {
         
         stream.writeBit(1);
         
         for (auto character : bString)
         {
            stream.writeBit(1);
            stream << character;
         }
         
         stream.writeBit(0);
         
         
         return stream;
      }

      friend PowerEncoding& operator >>
      ( 
         PowerEncoding& stream,
         BString& bString
      )
      {
         CHECK(stream.readBit() == true);

         bString.clear();
         Character character;
         while (stream.readBit() == true)
         {
            stream >> character;
            bString.push_back(character);
         }
         
         
         return stream;
         
      }
      
      // trim from start
      BString ltrim()
      {
         BString s = *this;
         s.erase(
            s.begin(),
            std::find_if(
               s.begin(),
               s.end(),
               [](Character ch)
               {
                  return !std::isspace(ch);
               }
            )
         );
         return s;
      }
      

      // trim from end (in place)
      BString rtrim()
      {
         BString s = *this;
         
         s.erase(
            std::find_if(
               s.rbegin(),
               s.rend(),
               [](Character ch)
               {
                  return !std::isspace(ch);
               }
            ).base(),
            s.end()
         );
         
         return s;
      }

      BString trim()
      {
         return ltrim().rtrim();
      }
      
      BString substr(size_t pos, size_t len = 0) const
      {
         BStringBase::const_iterator
            start  = cbegin() + pos;
            
         BStringBase::const_iterator
            end;
            
         if (len)
            end = start + len;
         else
            end = cend();
         return BString(start, end);
      }
      
      friend istream& getline(istream& in, BString& line)
      {
         string str;
         getline(in, str);
         line = str2wstr(str);
         return in;
      }
      
      static void writeEscaped(
         wostream& out,
         const Character& character
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
         default:
            if (character <= 0x001F)
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill(L'0')
                   << character;
            else if (character > 0x10FFFF)
            {
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill(L'0')
                   << 
                   ((character & 0xFFFF0000) >>
                       16);
               out << "\\u" 
                   << std::hex
                   << std::setw(4)
                   << std::setfill(L'0')
                   <<
                   (character & 0x0000FFFF);
            }
            else
               out << character;

         }
         
      }
   };



}

#endif


