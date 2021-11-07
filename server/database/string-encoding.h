#ifndef BEE_FISH_DATABASE__STRING_ENCODING_H
#define BEE_FISH_DATABASE__STRING_ENCODING_H

#include <iostream>
#include <string>
#include <vector>
#include <tgmath.h>
#include <math.h>
#include <bitset>

using namespace std;

namespace BeeFishDatabase
{

   class StringEncoding
   {
   public:
   
      virtual void writeBit(bool bit) = 0;
      
      virtual bool readBit() = 0;
      
      virtual bool peekBit() = 0;
      
   public:
      StringEncoding()
      {
      }
     
      StringEncoding& operator <<
      (const string& value)
      {
         writeBit(1);
         for (char c : value) {
            
            bitset<8> bits = c;
            for (int i = 0; i < 8; ++i) {
               writeBit(bits[i]);
            }
            
         }
         writeBit(0);
         return *this;
      }
     
      StringEncoding& operator >>
      (string& value)
      {
         throw runtime_error("Not implemented");
      }
   };
   
}
#endif