#ifndef BEE_FISH_B_STRING__DATA_H
#define BEE_FISH_B_STRING__DATA_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include "../power-encoding/power-encoding.h"

using namespace bee::fish::power_encoding;

namespace bee::fish::b_string {

   class BString;
   
   typedef unsigned char Byte;
   
   class Data : public vector<Byte>
   {
   public:
      inline static const size_t BitCount = 8;
   public:
      
      Data()
      {
      }
      
      template<typename T>
      Data(const T& source)
      {
         resize(sizeof(source));
         
         memcpy(data(), &source, size());
      }
      
      template<typename T>
      Data(const vector<T>& source)
      {
         resize(source.size() * sizeof(T));
         
         memcpy(data(), source.data(), size());
      }
      
      Data(const void* source, size_t len)
      {
         resize(len);
         memcpy(c_str(), source, size());
      }
      
      Data(const string& source) :
         Data(source.c_str(), source.size())
      {
      }
      
      // Implemented in misc.h
      Data(const char* source);
      
      // Implemented in misc.h
      Data(const BString& source);
      
      Data(const Data& source) :
         vector<Byte>(source)
      {
      }
      
      virtual char* c_str() const
      {
         return (char*)(data());
      }

      template<typename T>
      operator const T&() const
      {
         const T* destination =
            (const T*)data();
         
         return *destination;
      }
      
      template<typename T>
      operator T&()
      {
         T* destination =
            (T*)data();
         
         return *destination;
      }
      
      operator BString() const;
      
      // defined in base64.h
      // included from string.h
      BString toBase64() const;
      
      // defined in base64.h
      // included from string.h
      static Data fromBase64
      (const BString& data);

      
      BString md5() const;

      // sha3_512
      BString sha3() const;
      
      BString toHex() const;

      inline static Data fromRandom(
         size_t byteCount
      )
      {
         unsigned char buffer[byteCount];

         int rc = RAND_bytes(
            buffer,
            sizeof(buffer)
         );
      
         // unsigned long err = ERR_get_error();

         if (rc != 1)
         {
            // RAND_bytes failed
            throw runtime_error("Random bytes failed");
         
         }
      
         return Data(buffer, byteCount);
      }
      
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
      
      // Implemented in misc.h
      friend PowerEncoding& operator <<
      ( 
         PowerEncoding& stream,
         const Data& data
      );
         
   };
   
   
   
}

#endif


