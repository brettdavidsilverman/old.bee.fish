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
#include "config.h"

#ifdef SERVER
#include <filesystem>
#endif

#include "data.h"
#include "bit-stream.h"
#include "character.h"

#include "../parser/utf-8.h"

#ifdef SERVER
using namespace std::filesystem;
#endif

namespace BeeFishBString
{

   typedef vector<Character> BStringBase;

   class BString;

   // A string of variable length characters.
   // Can be created from wide string format,
   // and utf-8 format.
   // Once created, holds each character as a
   // power encoded vector of bits
   class BString : public BStringBase

   {

   public:
      // empty string
      BString()
      {
      }

      // standard copy constructor
      BString(const BString &source) : BStringBase(source)
      {
      }

      // from vector
      BString(const BStringBase &source) : BStringBase(source)
      {
      }

      // from data
      BString(const Data &source) : BString(fromData(source))
      {
      }

#ifdef SERVER
      // from path
      BString(const path &path) : BString(string(path))
      {
      }
#endif

      // from copy iterators
      BString(
          BStringBase::const_iterator first,
          BStringBase::const_iterator last) : BStringBase(first, last)
      {
      }

      static BString fromUTF8String(const std::string &str)
      {
         BeeFishParser::UTF8Character utf8;
         BString result;
         for (auto character : str)
         {
            if (utf8.match(character))
            {
               if (utf8.result() == true)
               {
                  result.push_back(utf8.character());
                  utf8.reset();
               }
            }
         }

         return result;
      }

      // from utf-8 string
      BString(const std::string &str) : BString(fromUTF8String(str))
      {
      }

      // c string
      BString(const char *cstring) : BString(std::string(cstring))
      {
      }

      // c string with length
      BString(const char *cstring, size_t len) : BString(std::string(cstring, len))
      {
      }

      BString(wstring wstr)
      {

         for (auto character : wstr)
         {
            Character c = character;
            this->push_back(c);
         }
      }

      // Wide c string
      BString(const wchar_t *wstr) : BString(wstring(wstr))
      {
      }

      Data toData() const
      {
         BitStream stream;

         stream << *this;

         return stream.toData();
      }

      // Stream indexable bits from data
      static BString fromData(const Data &source)
      {

         BitStream stream = BitStream::fromData(source);

         BString bString;

         stream >> bString;

         return bString;
      }

      std::string toUTF8() const
      {
         stringstream stream;
         stream << *this;
         return stream.str();
      }

      virtual ~BString()
      {
      }

      size_t length() {
         return size();
      }

      void push_back(const Character &character)
      {
         if (size())
         {
            Character &last = (*this)[size() - 1];
            if (last.isSurrogatePair(character))
            {
               last.joinSurrogatePair(character);
               return;
            }
         }
         BStringBase::push_back(character);
      }

      BString &operator+=(const BString &rhs)
      {
         for (auto character : rhs)
         {
            push_back(character);
         }
         return *this;
      }

      BString operator+(const BString &rhs) const
      {
         BString str(*this);
         for (auto character : rhs)
            str.push_back(character);
         return str;
      }

      virtual bool operator==(const char *rhs) const
      {

         BString comparison = rhs;

         return (*this == comparison);
      }

      virtual bool operator!=(const char *rhs) const
      {
         BString comparison(rhs);

         return (*this != comparison);
      }

      operator std::string() const
      {
         return toUTF8();
      }

      BString toLower() const
      {
         BString copy;
         for (const Character &c : *this)
            copy.push_back(
                tolower(c));

         return copy;
      }

      vector<BString> split(
          const Character &character) const
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

      friend ostream &operator<<(ostream &out, const BString &str)
      {
         str.write(out);
         return out;
      }

      virtual void write(ostream &out) const
      {
         for (const Character &character : *this)
         {
            out << character;
         }
      }

      virtual void writeEscaped(
          ostream &out) const
      {
         for (const Character &character : *this)
            writeEscaped(out, character);
      }

      friend PowerEncoding &operator<<(
          PowerEncoding &stream,
          const BString &bString)
      {

         stream.writeBit(1);

         for (auto character : bString)
         {
            stream << character;
         }

         stream.writeBit(0);

         return stream;
      }

      friend PowerEncoding &operator>>(
          PowerEncoding &stream,
          BString &bString)
      {
         CHECK(stream.readBit() == 1);

         bString.clear();
         Character character;

         while (stream.peekBit() == 1)
         {
            stream >> character;
            bString.push_back(character);
         }

         CHECK(stream.readBit() == 0);

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
                 }));
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
                 })
                 .base(),
             s.end());

         return s;
      }

      BString trim()
      {
         return ltrim().rtrim();
      }

      BString substr(size_t pos, size_t len = 0) const
      {
         BStringBase::const_iterator
             start = cbegin() + pos;

         BStringBase::const_iterator
             end;

         if (len)
            end = start + len;
         else
            end = cend();
         return BString(start, end);
      }

      friend istream &getline(istream &in, BString &line)
      {
         string str;
         getline(in, str);
         line = BString::fromUTF8String(str);
         return in;
      }

      static void writeEscaped(
          ostream &out,
          const Character &character)
      {
         character.writeEscaped(out);
      }

   };

   inline ostream &operator<<(ostream &out, const vector<Character> &characters)
   {
      BString str(characters);
      str.write(out);
      return out;
   }

   inline bool operator==(std::vector<BeeFishBString::Character> _1, const char *_2)
   {
      return BString(_1) == BString(_2);
   }

}

#endif
