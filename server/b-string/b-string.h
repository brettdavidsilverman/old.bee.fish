#ifndef BEE_FISH_B_STRING__B_STRING_H
#define BEE_FISH_B_STRING__B_STRING_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <cstring>

#include <openssl/md5.h>
#include "char.h"
#include "data.h"
#include "bit-stream.h"

namespace bee::fish::b_string {

   typedef vector<Char> BStringBase;
   
   class BString :
      public BStringBase
   
   {
   protected:
      size_t _size = 0;
      
   public:
      inline static const Char EndOfFile =
         UTF8Character::EndOfFile;
      
      // empty string
      BString()
      {
      }
      
      // standard copy constructor
      BString(const BString& source)
      {
         reserve(source.size());
         
         for (auto c : source)
         {
            push_back(c);
         }
         
         _size = source._size;
      }
      
      BString(const Data& source)
      {
         BString bstring = fromData(source);
         reserve(bstring.size());
         for (auto c : bstring)
         {
            push_back(c);
         }
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
               push_back(Char(utf8._character));
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
         for (const wchar_t& wc : wstring)
         {
            push_back((Char)wc);
         }
      }

      // Base string
      BString(const BStringBase& source) :
         BStringBase(source)
      {
      }
      
      // Stream indexable bits from data
      static BString fromData(const Data& source)
      {

         BitStream stream(source);
         
         BString bstring;
         
         stream >> bstring;
         
         return bstring;
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
         for (const Char& character : rhs)
         {
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
      
      operator std::string () const
      {
         stringstream stream;

         stream << *this;

         return stream.str();
      }
      

      BString toLower() const
      {
         BString copy;
         for (const Char& c : *this)
            copy.push_back(
               c.toLower()
            );
            
         return copy;
      }

      vector<BString> split(const Char& character) const
      {
         BString segment;
         vector<BString> segments;

         for (const Char& c : *this)
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
  
      const char* c_str () const
      {
         if (size())
            return 
               (const char*)&((*this)[0]);
         else
            return nullptr;
      }
      
      
      
      friend ostream& operator <<
      (ostream& out, const BString& str)
      {
         str.write(out);
         return out;
      }
      
      virtual void write(ostream& out) const
      {
 
         for ( const Char& character : *this )
         {
            out << character;
         }
      }
      
      virtual void writeEscaped(
         ostream& out
      ) const
      {
         for (const Char& character : *this)
            character.writeEscaped(out);
      }

      virtual void push_back(const Char& character)
      {
         if ( size() )
         {
            Char& last = (*this)[size() - 1];
            if ( last.isSurrogatePair(
                    character
                 ) )
            {
               last.joinSurrogatePair(
                  character
               );
            }
            else
               BStringBase::push_back(character);
         }
         else
            BStringBase::push_back(character);
      }
      
      friend PowerEncoding& operator <<
      ( 
         PowerEncoding& stream,
         const BString& bstring
      )
      {
         stream.writeBit(1);
         
         for (const Char& character : bstring)
         {
            stream << character;
         }
         
         stream.writeBit(0);
         
         return stream;
      }

      friend PowerEncoding& operator >>
      ( 
         PowerEncoding& stream,
         BString& bstring
      )
      {
         CHECK(stream.readBit() == true);

         bstring.clear();
         
         while (stream.peekBit())
         {
            Char character;
            stream >> character;
            bstring.push_back(character);
         }
         
         CHECK(stream.readBit() == false);
         
         return stream;
         
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


