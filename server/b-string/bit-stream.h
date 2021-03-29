#ifndef BEE_FISH_B_STRING__BIT_STREAM_H
#define BEE_FISH_B_STRING__BIT_STREAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>

#include "../power-encoding/power-encoding.h"

#include "char.h"
#include "data.h"

namespace bee::fish::b_string {

   class BitStream :
      public PowerEncoding
   {
   protected:
      Data& _data;
      size_t _bytePosition = 0;
      int _bitPosition = 0;

   public:
   
      BitStream(Data& data) :
         _data(data)
      {
      }
      
      
      virtual void writeBit(bool bit)
      {
         throw logic_error("Not implemented");
      }
      
      virtual bool readBit()
      {
         bool bit = peekBit();
         
         if (_bitPosition == 0)
         {
            _bitPosition = 7;
            ++_bytePosition;
         }
         
         return bit;
         
      }
      
      virtual bool peekBit()
      {
         const Byte& byte = _data[_bytePosition];
         bool bit = (byte & (1 << _bitPosition));
         return bit;
      }

         
   };
   
}

#endif


