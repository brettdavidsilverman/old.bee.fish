#ifndef BEE_FISH_DATABASE__STRING_ENCODING_H
#define BEE_FISH_DATABASE__STRING_ENCODING_H

#include <iostream>
#include <string>
#include <vector>
#include <tgmath.h>
#include <math.h>


using namespace std;

namespace bee::fish::database
{


   //vector<void(StringEncoding& encoding)> chars;
   
   class StringEncoding
   {
   protected:
      typedef void(*writeBits)(StringEncoding&);
   
      static vector<writeBits> chars;
      virtual void writeBit(bool bit)
      {
         cout << (bit ? '1' : '0');
      }
      
      virtual bool readBit()
      {
         char c;
         cin >> c;
         return (c != '0');
      }
      
      virtual bool peekBit()
      {
         char c = (char)(cin.peek());
         return (c != '0');
      }
      
   public:
      StringEncoding()
      {
      }
     
      StringEncoding& 
      operator << (const string& value)
      {
         
         for (const char c : value)
         {
           // chars[(unsigned char)c](*this);
            
            unsigned char uc = c;
            unsigned char mask = 0b10000000;
            for (int i = 0; i < 8; i++)
            {
               bool bit = (uc & (mask >> i));
               writeBit(bit);
            }
            
         }
         
         writeBit(false);
         
         return *this;
      }
     
      StringEncoding& operator >> (string& value)
      {
      
         throw runtime_error("Not implemented yet");
      }
      
   };
   
}
#endif