#ifndef BEE_FISH_B_STRING__STREAM_H
#define BEE_FISH_B_STRING__STREAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <functional>
#include <unistd.h>

#include "../power-encoding/power-encoding.h"
#include "../misc/pagesize.h"

#include "b-string.h"

using namespace BeeFishPowerEncoding;

namespace BeeFishBString {

   typedef std::basic_streambuf<Character> StreamBuf;
   typedef std::basic_ostream<Character> OStream;

   class BStream :  
      protected StreamBuf,
      public OStream
   {
   protected:
      std::vector<unsigned char> _bytes;

   public:
      typedef std::function<void(const Data& buffer)> OnBuffer;
      OnBuffer _onbuffer = nullptr;
      size_t _bufferSize;
   public:
      
      BStream(
         size_t bufferSize = getpagesize()
      ) :
         OStream(this),
         _bufferSize(bufferSize)
      {
      }

      BStream(const BStream& copy) :
         StreamBuf(copy),
         OStream(this),
         _bytes(copy._bytes),
         _bufferSize(copy._bufferSize)
      {

      }

      virtual ~BStream() {
         flush();
      }


      virtual void push_back(Character c) {
         Byte* bytes = (Byte*)(&c);
         _bytes.push_back(bytes[0]);
         _bytes.push_back(bytes[1]);
         _bytes.push_back(bytes[2]);
         _bytes.push_back(bytes[3]);
         if (size() >= _bufferSize)
            onBuffer();
      }      

      virtual void flush() {
         if (size())
            onBuffer();
      }

      virtual void setOnBuffer(OnBuffer onbuffer) {
         _onbuffer = onbuffer;
      }

      StreamBuf::int_type overflow(StreamBuf::int_type c) override
      {
         push_back((Character)c);
         return 0;
      }

      virtual size_t size() const {
         return _bytes.size();
      }

      inline friend OStream& operator << (OStream& out, const char* value) {
         BString string(value);
         out << string;
         return out;
      }

   protected:
      virtual void onBuffer() {

         if (_onbuffer) {

            Data data(_bytes);

            _onbuffer(data);

         }

         _bytes.clear();
      } 


   };

   
}

#endif


