#ifndef BEE_FISH_B_STRING__STREAM_H
#define BEE_FISH_B_STRING__STREAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <functional>
#include <unistd.h>

#include "../power-encoding/power-encoding.h"
#include "../misc/pagesize.h"

#include "b-string.h"

using namespace BeeFishPowerEncoding;

namespace BeeFishBString {

   template<class StreamOf>
   class BStream :  
      protected std::streambuf,
      public std::ostream,
      public StreamOf
   {
   public:
      typedef typename  StreamOf::ValueType ValueType;
      typedef std::function<void(const StreamOf& buffer)> OnBuffer;
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
         StreamOf(copy),
         _bufferSize(copy._bufferSize)
      {

      }

      virtual ~BStream() {
         flush();
      }


      virtual void push_back(const ValueType& character) {
         StreamOf::push_back(character);
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
         push_back(c);
         return 0;
      }

      virtual size_t size() const {
         return StreamOf::size();
      }

   protected:
      virtual void onBuffer() {
         if (_onbuffer)
            _onbuffer(*this);

         StreamOf::clear();
      } 


   };

   typedef BStream<BString> BStringStream;
   typedef BStream<Data> DataStream;
   
}

#endif


