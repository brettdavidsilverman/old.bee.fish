#ifndef BEE_FISH_B_STRING__BITS_H
#define BEE_FISH_B_STRING__BITS_H

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctype.h>
#include "char.h"
#include "data.h"

namespace bee::fish::b_string {

   class Bits :
      public vector<bool>
   {
         
   public:
   
      Bits()
      {
      }
      
      Bits(const vector<bool>& bits) :
         vector<bool>(bits)
      {
      }
      
      Data toData()
      {

         Data output;
         Byte byte = 0;
         unsigned int i = 0;
         Char::Value mask =
            1 << (Data::BitCount - 1);
         for (bool bit : *this)
         {
            if (bit)
               byte |= mask;
                  
            if (++i < 8)
               mask = mask >> 1;
            else
            {
               output.push_back(
                  byte
               );
               byte = 0;
               i = 0;
               mask = 1 << (Data::BitCount - 1);
            }
         }
            
         if (i > 0)
         {
            output.push_back(byte);
         }
         
         return output;
            
      }
         
      static Bits fromData
      (const Data& data)
      {
         Bits bitString;
            
         for ( Byte byte : data )
         {
            for ( int i = (Data::BitCount - 1);
                  i >= 0;
                  --i )
            {
               Byte mask = (1 << i);
               bool bit = (byte & mask);
               bitString.push_back(bit);
            }
         }
            
         return bitString;
      }
         
      friend ostream& operator <<
      (ostream& out, const Bits& bitString)
      {
         bitString.write(out);
         
         return out;
      }
      
      virtual void write(ostream& out) const
      {
         for (bool bit : *this)
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


