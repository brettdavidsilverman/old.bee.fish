#ifndef BEE_FISH_B_STRING__DATA_H
#define BEE_FISH_B_STRING__DATA_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#include "b-string.h"

namespace bee::fish::b_string {

   typedef unsigned char Byte;
   
   class Data : public vector<Byte>
   {
   public:
      inline static const size_t BitCount = 8;
   public:
      
      Data()
      {
      }
      
      Data(const vector<Byte>& data) :
         vector<Byte>(data)
      {
      }
      
      Data(const Byte* data, size_t len)
      {
         for (size_t i = 0; i < len; ++i)
         {
           // cerr << data[i] << "#";
            push_back(data[i]);
         }
      }
      
      Data(const char* data)
      {
         size_t len = strlen(data);
         for (size_t i = 0; i < len; ++i)
         {
            push_back(data[i]);
         }
      }
      
      Data(const char* data, size_t len)
      {
         for (size_t i = 0; i < len; ++i)
         {
            push_back(data[i]);
         }
      }
      
      Data(const string& data)
      {
         size_t len = data.length();
         for (size_t i = 0; i < len; ++i)
         {
            push_back(data[i]);
         }
      }
      
      Data(const BString& data)
      {
         const char* charData = data.c_str();
         const Byte* byteData =
            (const Byte*)(charData);
         size_t byteCount = data.size() * Char::BytesPerChar;
         
         for ( size_t i = 0;
               i < byteCount;
               ++i )
         {
            push_back(
               byteData[i]
            );
         }
      }
      
      Data(const Data& source) :
         vector<Byte>(source)
      {
      }
      
      virtual char* c_str() const
      {
         if (size())
            return (char*)&((*this)[0]);
         else
            return nullptr;
      }
      
      virtual operator string() const
      {
         string result;
         char character;
         for ( size_t i = 0;
               i < size();
               ++i)
         {
            character = (*this)[i];
            result.push_back(
               character
            );
         }
         return result;
      }
      
      // defined in base64.h
      // included from string.h
      BString toBase64() const;
      
      // defined in base64.h
      // included from string.h
      static Data fromBase64
      (const BString& data);

      
      
      friend ostream& operator <<
      (ostream& out, const Data& data)
      {
         data.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out) const
      {
         for (Byte byte : *this)
         {
            out << (char)byte;
         }
      }
         
   };
   
}

#endif


