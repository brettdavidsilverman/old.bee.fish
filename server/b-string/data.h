#ifndef BEE_FISH_B_STRING__DATA_H
#define BEE_FISH_B_STRING__DATA_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include "b-string.h"

#ifdef SERVER
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif
#include "../misc/pagesize.h"

#include "../power-encoding/power-encoding.h"

using namespace BeeFishPowerEncoding;

namespace BeeFishBString {

   class BString;
   
   typedef unsigned char Byte;
   
   class Data
   {
   protected:
      Byte* _readWrite = nullptr;
      const Byte* _data;
      size_t _size = 0;

   public:
      typedef Byte ValueType;

      Data() : _readWrite(nullptr), _data(nullptr), _size(0)
      {
      }

      static Data create(size_t size = 0) {

         Data data;

         if (size == 0)
            size = getpagesize();

         data._readWrite = (Byte*)malloc(size);
         data._data = data._readWrite;
         data._size = size;
         
         return data;
      }

      virtual ~Data() {
         if (_readWrite)
            free(_readWrite);
      }
      
      template<typename T>
      Data(const T& source) : _data((const Byte*)&source), _size(sizeof(T))
      {
      }
      
      template<typename T>
      Data(const vector<T>& source) : _data(source.data()), _size(source.size() * sizeof(T))
      {
      }
      
      Data(const void* source, size_t len) : _data((Byte*)source), _size(len)
      {
      }
      
      Data(const Byte* source, size_t len) : _data(source), _size(len)
      {
      }

      Data(const Byte* source, size_t len, bool copy) : 
         _readWrite((Byte*)malloc(len)),
         _data(_readWrite), 
         _size(len)
      {
         memcpy(_readWrite, source, _size);
      }

      Data(const string& source) :
         Data(source.c_str(), source.size())
      {
      }
      
      // Implemented in misc.h
      Data(const char* source) : Data(source, strlen(source)) {
         
      }
      
      // Implemented in misc.h
      Data(const BString& source);
      
      Data(const Data& source) :
         _readWrite(nullptr),
         _data(source._data),
         _size(source._size)
      {
      }

      Data(Data& source) :
         _data(source._data),
         _size(source._size)
      {
         if (source._readWrite) {
            _readWrite = source._readWrite;
            source._readWrite = nullptr;
         }
      }

      const Byte* data() const {
         return _data;
      }

      Byte* data() {
         return _readWrite;
      }

      const char* c_str() const {
         return (const char*) _data;
      }

      virtual size_t size() const {
         return _size;
      }

      template<typename T>
      operator const T&() const
      {
         const T* destination =
            (const T*)_data;
         
         return *destination;
      }

      template<typename T>
      operator T() const
      {
         const T* destination =
            (const T*)_data;
         
         return *destination;
      }

      bool operator == (const Data& rhs) {
         if (rhs._size != _size)
            return false;
         return (memcmp(rhs._data, _data, _size) == 0);
      }

      void clear() {
         
      }
      
      operator BString() const;

      BString toHex() const;
      

      // defined in base64.h
      // included from string.h
      BString toBase64() const;

      // defined in base64.h
      // included from string.h
      static Data fromBase64
      (const BString& data);

#ifdef SERVER
      
      BString md5() const;

      // sha3_512
      BString sha3() const;
      

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
#endif

      friend ostream& operator <<
      (ostream& out, const Data& data)
      {
         data.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out) const
      {
         for (size_t i = 0; i < _size; ++i)
         {
            Byte byte = _data[i];
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


