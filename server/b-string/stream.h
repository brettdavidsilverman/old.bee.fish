#ifndef BEE_FISH_B_STRING__STREAM_H
#define BEE_FISH_B_STRING__STREAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <functional>
#include <unistd.h>

#include "../power-encoding/power-encoding.h"
#include "b-string.h"

using namespace BeeFishPowerEncoding;

namespace BeeFishBString {

   
   class BStringStream : public BString
   {
   public:
      typedef std::function<void(const BString& buffer)> OnBuffer;
      OnBuffer _onbuffer = nullptr;
      size_t _bufferSize;
   public:
      
      BStringStream(
         size_t bufferSize = getpagesize()
      ) :
         _bufferSize(bufferSize)
      {
      }
      
      virtual void push_back(const Character& character) {
         BString::push_back(character);
         if (size() >= _bufferSize)
            _onBuffer();
      }      

      virtual void flush() {
         if (size())
            _onBuffer();
      }

      virtual void setOnBuffer(OnBuffer onbuffer) {
         _onbuffer = onbuffer;
      }

   protected:
      virtual void _onBuffer() {
         if (_onbuffer)
            _onbuffer(*this);

         clear();
      } 


   };
   
   
   
}

#endif


