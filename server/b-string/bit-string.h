#ifndef BEE_FISH_B_STRING__BIT_STRING_H
#define BEE_FISH_B_STRING__BIT_STRING_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>

#include "data.h"

namespace bee::fish::b_string {

   class BitString
   {
   protected:
      vector<bool> _bits;
      Data _data;
         
   public:
      static BitString fromBits
      (const vector<bool>& bits)
      {
         BitString bitString;
         bitString._bits = bits;

         Byte byte = 0;
         unsigned int i = 0;
         Char::Value mask =
            1 << (Data::BitCount - 1);
         for (bool bit : bits)
         {
            if (bit)
               byte |= mask;
                  
            if (++i < 8)
               mask = mask >> 1;
            else
            {
               bitString._data.push_back(
                  byte
               );
               byte = 0;
               i = 0;
               mask = 1 << (Data::BitCount - 1);
            }
         }
            
         if (i > 0)
         {
            bitString._data.push_back(byte);
         }
         
         return bitString;
            
      }
         
      static BitString fromData
      (const Data& data)
      {
         BitString bitString;
            
         bitString._data = data;
            
         for ( Byte byte : data )
         {
            for ( int i = (Data::BitCount - 1);
                  i >= 0;
                  --i )
            {
               Byte mask = (1 << i);
               bool bit = (byte & mask);
               bitString._bits.push_back(bit);
            }
         }
            
         return bitString;
      }
         
      const Data& data() const
      {
         return _data;
      }
         
      const vector<bool>& bits() const
      {
         return _bits;
      }
      
      friend ostream& operator <<
      (ostream& out, const BitString& bitString)
      {
         bitString.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out) const
      {
         for (bool bit : _bits)
         {
            if (bit)
               out << '1';
            else
               out << '0';
         }
      }
         
   };
   
}

#endif


