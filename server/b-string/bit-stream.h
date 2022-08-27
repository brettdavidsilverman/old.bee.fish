#ifndef BEE_FISH_B_STRING__BIT_STREAM_H
#define BEE_FISH_B_STRING__BIT_STREAM_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include <bitset>

#include "../power-encoding/power-encoding.h"
#include "b-string.h"
#include "data.h"
#include "character.h"

namespace BeeFishBString {

   // A byte aligned stream of bits
   // using a Data member as storage
   // and PowerEncoding as the encoding
   // mechanism

   // This defines the readBit and writeBit
   // functions of PowerEncodong, to append
   // a bit to the current byte.
   class BitStream :
      public PowerEncoding,
      public vector<bool>
   {
   private:
      vector<bool>::const_iterator _it;
      size_t _count = 0;
      
   public:
   
      BitStream()
      {
         _it = cend();
      }
      
      BitStream(const BitStream& bitStream) :
         vector<bool>(bitStream)
      {
          _it = cbegin();
      }
      
      BitStream(const BString& source) {
         for (auto character : source) {
            for (auto bit : character) {
               push_back(bit);
            }
         }
      }

      BitStream(const Data& data)
      {
         // [0,1,2,3,4,5,6,7]
         const Byte* _data = data._data;
         const size_t _size = data.size();

         long int count = 0;

         for (size_t i = 0; i < _size; ++i) {
            Byte byte = _data[i];
            bitset<8> bits = byte;
            for ( int i = 0;
                  i < 8;
                  ++i )
            {
               bool bit = bits[7 - i];

               if (count >= 0) {

                  if (bit)
                     ++count;
                  else
                     --count;

                  if (count >= 0)
                     push_back(bit);

               }

            }
         }
         
         _it = cbegin();
    
      }

      static BitStream fromData(const Data& data)
      {
     
         return BitStream(data);
      }
      
      Data toData() const
      {
         std::vector<Byte> data;
         
         int bitCount = 0;
         bitset<8> bits = 0;
         
         for (auto bit : *this)
         {
            bits[7 - bitCount] = bit;
            if (++bitCount == 8)
            {
               Byte byte =
                  bits.to_ulong();
                  
               data.push_back(byte);
               bits = 0;
               bitCount = 0;
            }
         }
         
         if (bitCount > 0)
         {
            Byte byte =
               bits.to_ulong();
                  
            data.push_back(byte);
         }
         
         return Data(data.data(), data.size(), true);
      }
      
      virtual void writeBit(bool bit)
      {
         push_back(bit);
      }
      
      virtual bool readBit()
      {
      
         bool bit = peekBit();
 
         if (bit)
            ++_count;
         else
            --_count;
            
         ++_it;
         
         return bit;
         
      }
      
      virtual bool peekBit()
      {
         if (_it == cend())
            throw runtime_error("BitStream Past end of file");
            
         bool bit = *_it;
         
         return bit;
      }

      void reset()
      {
         _it = cbegin();
      }
      
      size_t count()
      {
         return _count;
      }
      
   };
   
}

#endif


