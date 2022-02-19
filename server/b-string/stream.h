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

   class BStream :  
      protected std::streambuf,
      public std::ostream
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
         std::ostream(this),
         _bufferSize(bufferSize)
      {
      }

      BStream(const BStream& copy) :
         std::streambuf(copy),
         std::ostream(this),
         _bytes(copy._bytes),
         _bufferSize(copy._bufferSize)
      {

      }

      virtual ~BStream() {
         flush();
      }


      virtual void push_back(Byte byte) {
         _bytes.push_back(byte);
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

      int overflow(int c) override
      {
         push_back((Byte)c);
         return 0;
      }

      virtual size_t size() const {
         return _bytes.size();
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


