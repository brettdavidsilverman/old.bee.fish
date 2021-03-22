#ifndef BEE_FISH_B_STRING__B_STRING_H
#define BEE_FISH_B_STRING__B_STRING_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <openssl/md5.h>
#include "char.h"
#include "base64.h"

namespace bee::fish::b_string {

   typedef vector<Char> BStringBase;
   
   class BString :
      public BStringBase
   
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
               push_back(Char(utf8._character));
               utf8.reset();
            }
         }

      }
      
      BString(const char* cstring) :
         BString(std::string(cstring))
      {
      }
      
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
      
      // utf-8 character
      BString(const Char& character)
      {
         push_back(character);
      }
   
      virtual ~BString()
      {
      }
      
      BString& operator += (const string& rhs)
      {
         BString string(rhs);
         insert(end(), string.begin(), string.end());
         return *this;
      }
      
      BString operator + (const string& rhs) const
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
      
      BString md5() const
      {
         //std::string str = *this;

         unsigned char result[MD5_DIGEST_LENGTH];
      
         MD5(
            (unsigned char*)(this),
            sizeof(Char) * size(),
            result
         );

         std::stringstream sout;
      
         sout << std::hex << std::setfill('0');
      
         for(long long c: result)
         {
            sout << std::setw(2) << (long long)c;
         }
      
         return sout.str();
      
      }
      
      BString base64() const
      {
         char* out;
         size_t len;
         const base64::byte*
            bytes = 
               (const base64::byte*)
               c_str();
         
         base64::_encode(
            bytes,
            size() * sizeof(Char::Value),
            &out,
            &len
         );
      
         BString result(out, len);
         free(out);
         return result;
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
      
      BString toHex() const
      {
      
         BString hex = "";
      
         for ( const Char& character : *this )
         {
            hex += character.toHex();
         }
      
         return hex;
      }
      
      static BString fromHex(const BString& hex)
      {
         BString result;
         int i = 0;
         string part;
         
         for (const Char& c : hex)
         {
            part.push_back(c);
            if (++i == 8)
            {
               Char character =
                  Char::fromHex(part);
               result.push_back(character);
               i = 0;
               part.clear();
            }
         }
         
         if (i > 0)
         {
            cerr << "****" << part << endl;
            Char character =
               Char::fromHex(part);
            result.push_back(character);
         }
         
         return result;
         
      }
      
      vector<BString> split(const Char& character) const
      {
         BString segment;
         std::vector<BString> segments;

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
      
      virtual void push_back(
         const Char& character
      )
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


