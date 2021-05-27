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
#include <openssl/md5.h>

#include "character.h"
#include "data.h"
#include "bit-stream.h"

namespace bee::fish::b_string {

   typedef basic_string<Character> BStringBase;

   class BString;
   
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
      
      // from basic_string
      BString(const BStringBase& source) :
         BStringBase(source)
      {
      }
      
      // from data
      BString(const Data& source) :
         BString(fromData(source))
      {
      }
      
      // from utf-8 string 
      BString(const std::string& str)
      {
         UTF8Character utf8;
         for (char c : str)
         {
  
            utf8.match(c);
            if (utf8._result)
            {
               Character character(
                  utf8.value()
               );
               push_back(character);
               utf8.reset();
            }
            
         }

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
      
      
      // wide string 
      BString(const std::wstring& wstring)
      {
         for (auto wc : wstring)
         {
            push_back(Character(wc));
         }
      }

      // Stream indexable bits from data
      static BString fromData(const Data& source)
      {

         BitStream stream(source);
         
         BString b_string;
         
         stream >> b_string;
         
         return b_string;
      }
      
      Data toData() const
      {
         BitStream stream;
         
         stream << *this;
         
         return stream._data;
      }
      
      virtual ~BString()
      {
      }
      /*
      BString& operator += (const BString& rhs)
      {
         for (auto character : rhs)
         {
            BStringBase::
            push_back(character);
         }
         return *this;
      }
      */
      BString operator + (const BString& rhs) const
      {
         BString str(*this);
         str += rhs;
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
         stringstream stream;

         stream << *this;

         return stream.str();
      }
      

      BString toLower() const
      {
         BString copy;
         for (const Character& c : *this)
            copy.push_back(
               c.toLower()
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
      
      friend ostream& operator <<
      (ostream& out, const BString& str)
      {
         str.write(out);
         return out;
      }
      
      virtual void write(ostream& out) const
      {
         for ( auto character : *this )
         {
            out << character;
         }
      }
      
      virtual void writeEscaped(
         ostream& out
      ) const
      {
         for (const Character& character : *this)
            character.writeEscaped(out);
      }
/*
      void push_back(const Character& character)
      {
         if ( size() )
         {
            Character& last =
               (*this)[size() - 1];
            
            Character::Value value = last;
            cerr << "*7";
            if ( last.isSurrogatePair(
                    value
                 ) )
            {
               cerr << "*8";
               last.joinSurrogatePair(
                  value
               );
               
               return;
            }
            
         }
         
         cerr << "*10";
         BStringBase::push_back(character);
            
         cerr << "*12";
      }
      */
      friend PowerEncoding& operator <<
      ( 
         PowerEncoding& stream,
         const BString& b_string
      )
      {
         stream.writeBit(1);
         
         for (auto character : b_string)
         {
            stream << character;
         }
         
         stream.writeBit(0);
         
         return stream;
      }

      friend PowerEncoding& operator >>
      ( 
         PowerEncoding& stream,
         BString& b_string
      )
      {
         CHECK(stream.readBit() == true);

         b_string.clear();
         
         while (stream.peekBit())
         {
            Character character;
            stream >> character;
            b_string.push_back(character);
         }
         
         CHECK(stream.readBit() == false);
         
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
      
   };

   template<class T>
   inline bool getline(T& in, BString& line)
   {
      std::string str;
      bool result =
         getline(in, str);
         
      if (result)
         line = str;
         
      return result;
   }

}

#endif


