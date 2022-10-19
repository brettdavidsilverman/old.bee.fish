#ifndef BEE_FISH_B_STRING__STREAM_H
#define BEE_FISH_B_STRING__STREAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <functional>

#include "../power-encoding/power-encoding.h"
#include "../misc/pagesize.h"

#include "b-string.h"

using namespace BeeFishPowerEncoding;

namespace BeeFishBString {

   typedef std::vector<unsigned char> Bytes;

   class 
   BStream :
      public Bytes,
      private std::streambuf,
      public std::ostream
   {
   protected:
      size_t _totalSize = 0;

   public:
      typedef std::function<void(const Data& buffer)> OnBuffer;
      OnBuffer _onbuffer = nullptr;
      size_t _bufferSize;
   public:
      
      BStream(
         size_t bufferSize = getPageSize()
      ) :
         std::ostream(this),
         _bufferSize(bufferSize)
      {
         reserve(_bufferSize);
      }

      BStream(const BStream& copy) :
         Bytes(copy),
         std::ostream(this),
         _onbuffer(copy._onbuffer),
         _bufferSize(copy._bufferSize)
      {
         reserve(_bufferSize);
      }

      virtual ~BStream() {
      }


      int overflow(int c) override
      {
         push_back((unsigned char)c);
         return 0;
      }

      virtual void push_back(unsigned char c) {
         Bytes::push_back(c);
         if (size() >= _bufferSize)
            onBuffer();
      }      

      virtual void flush() {
         if (size() > 0)
            onBuffer();
      }

      virtual void setOnBuffer(OnBuffer onbuffer) {
         _onbuffer = onbuffer;
      }

      virtual void write(const Data& data) {
         write((const char*)data._data, data.size());
      }

      virtual void write(const char* data, size_t size) {
         for (size_t i = 0; i < size; ++i) {
            push_back((char)data[i]);
         }
      }

      inline friend BStream& operator << (BStream& out, std::string value) {
         const Data data(value);
         out.write(data);
         return out;
      }

      inline friend BStream& operator << (BStream& out, const char* value) {
         for (const char *pc = value; *pc != 0; ++pc) {
            out.push_back(*pc);
         }
         return out;
      }

      inline friend BStream& operator << (BStream& out, BeeFishBString::Character character) {
         const std::string chars = getChars(character);
         
         for (const char c : chars)
            out.push_back(c);

         return out;
      }

      inline friend BStream& operator << (BStream& out, BeeFishBString::BString bstring) {
         for (const Character& character : bstring)
            out << character;
         return out;
      }

      inline friend BStream& operator << (BStream& out, int value) {
         (ostream&)out << value;
         return out;
      }

      inline friend BStream& operator << (BStream& out, size_t value) {
         (ostream&)out << value;
         return out;
      }

      virtual size_t totalSize() {
         return _totalSize;
      }

   protected:

      virtual void onBuffer() {

         _totalSize += size();

         if (_onbuffer) {

            const Data _data(data(), size(), false);

            _onbuffer(_data);

         }


         Bytes::clear();
         reserve(_bufferSize);
      } 

      virtual void clear() {
         Bytes::clear();
      }



   };

   
}

#endif