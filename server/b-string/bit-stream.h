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
   private:
      Data __data;
      size_t _bytePosition = 0;
      int _bitPosition = 7;
      
   public:
      Data& _data;

   public:
   
      BitStream(Data& data) :
         _data(data)
      {
      }
      
      BitStream() :
         _data(__data)
      {
         _data.push_back(0);
      }
      
      virtual void writeBit(bool bit)
      {
         if (_data.size() <= _bytePosition)
            _data.push_back(0);
            
         if (bit)
         {
            Byte mask = (1 << _bitPosition);
            Byte& byte = _data[_bytePosition];
 
            byte |= mask;
         }
         
         if (--_bitPosition == -1)
         {
            _bitPosition = 7;
            ++_bytePosition;
         }
      }
      
      virtual bool readBit()
      {
         bool bit = peekBit();
         
         if (_bitPosition == 0)
         {
            _bitPosition = 7;
            ++_bytePosition;
         }
         else
            --_bitPosition;
            
         return bit;
         
      }
      
      virtual bool peekBit()
      {
         const Byte& byte = _data[_bytePosition];
         Byte mask = (1 << _bitPosition);
         bool bit = (byte & mask);
         return bit;
      }

      void reset()
      {
         _bytePosition = 0;
         _bitPosition = 7;
      }
      
   };
   
}

#endif


