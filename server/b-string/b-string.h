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
   /*
   class BStringBase :
      public b_string
   {
   public:
      bool _isNull = false;
      BStringBase()
      {
      }
      
      BStringBase(
         const b_string& source,
         bool isNull = false
      ) :
         b_string(source),
         _isNull(isNull)
      {
      }
   };
   */
   class BString;
   
   class BString :
      public BStringBase
   
   {
   protected:
      size_t _size = 0;
         
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
      
      BString(const Data& source)
      {
         BString b_string = fromData(source);
         reserve(b_string.size());
         for (auto c : b_string)
         {
            push_back(c);
         }
      }
      
      // utf-8 string 
      BString(const std::string& str)
      {
         UTF8Character utf8;
         for (auto c : str)
         {
            utf8.match(c);
            if (utf8._result)
            {
               Character character =
                  utf8._character;
               BStringBase::
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
            push_back((Character)wc);
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
      
      BString& operator += (const BString& rhs)
      {
         for (auto character : rhs)
         {
            BStringBase::
            push_back(character);
         }
         return *this;
      }
      
      BString operator + (const BString& rhs) const
      {
         BString str(*this);
         str += rhs;
         return str;
      }
      
      virtual bool operator == (const char* rhs) const
      {
         BString comparison(rhs);
         
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
  /*
      const char* c_str () const
      {
         if (size())
            return 
               (const char*)&((*this)[0]);
         else
            return nullptr;
      }
      */
      
      
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

      virtual void push_back(const Character& character)
      {
         if ( size() )
         {
            auto lastIt = end();
            --lastIt;
            
            Character& last = *lastIt;
            
            if ( last.isSurrogatePair(
                    character
                 ) )
            {
               last.joinSurrogatePair(
                  character
               );
            }
            else
               BStringBase
               ::push_back(character);
         }
         else
            BStringBase::push_back(character);
            
      }
      
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
   /*
   inline const BString Null(
      BStringBase(
         {}, true
      )
   );
   */
}

#endif


