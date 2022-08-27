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

   typedef std::basic_streambuf<char> StreamBuf;
   typedef std::basic_ostream<char> OStream;

   class BStream :  
      protected StreamBuf,
      public OStream
   {
   protected:
      std::vector<unsigned char> _bytes;
      size_t _totalSize = 0;

   public:
      typedef std::function<void(const Data& buffer)> OnBuffer;
      OnBuffer _onbuffer = nullptr;
      size_t _bufferSize;
   public:
      
      BStream(
         size_t bufferSize = getPageSize()
      ) :
         OStream(this),
         _bufferSize(bufferSize)
      {
         _bytes.reserve(_bufferSize);
      }

      BStream(const BStream& copy) :
         StreamBuf(copy),
         OStream(this),
         _bytes(copy._bytes),
         _onbuffer(copy._onbuffer),
         _bufferSize(copy._bufferSize)
      {
         _bytes.reserve(_bufferSize);
      }

      virtual ~BStream() {
//         flush();
      }


      virtual void push_back(unsigned char c) {
         _bytes.push_back(c);
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

      StreamBuf::int_type overflow(StreamBuf::int_type c) override
      {
         push_back(c);
         return 0;
      }

      virtual size_t size() const {
         return _bytes.size();
      }

/*
      inline friend OStream& operator << (OStream& out, const char* value) {
         BString string(value);
         out << string;
         return out;
      }
*/
   protected:
      virtual void onBuffer() {

         if (_onbuffer) {

            const Data data(_bytes);

            _onbuffer(data);

            _totalSize += data.size();
         }

         _bytes.clear();
      } 


   };

   
}

#endif


