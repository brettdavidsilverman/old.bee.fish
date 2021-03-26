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
      
      // utf-8 character
      BString(const Char& character)
      {
         push_back(character);
      }
   
      // Basic string
      BString(const BStringBase& source) :
         BStringBase(source)
      {
      }
      
      // vector of bytes
      BString(const vector<unsigned char>& bytes)
      {
         UTF8Character utf8;
         
         for (const unsigned char& uc : bytes)
         {
            char c = (char)uc;
            utf8.match(c);
            if (utf8._result)
            {
               push_back(Char(utf8._character));
               utf8.reset();
            }
         }

      }
      
      virtual ~BString()
      {
      }
      
      BString& operator += (const BString& rhs)
      {
         BString string(rhs);
         insert(end(), string.begin(), string.end());
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
            Char character =
               Char::fromHex(part);
            result.push_back(character);
         }
         
         return result;
         
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
        // return (const char*)
        //    BStringBase::c_str();
     
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


